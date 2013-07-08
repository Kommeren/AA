/**
 * @file steiner_network.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-06-24
 */
#ifndef STEINER_NETWORK_HPP
#define STEINER_NETWORK_HPP

#include <boost/graph/kruskal_min_spanning_tree.hpp>

#include "paal/iterative_rounding/iterative_rounding.hpp"
#include "paal/iterative_rounding/ir_components.hpp"
#include "paal/iterative_rounding/lp_row_generation.hpp"
#include "paal/iterative_rounding/steiner_network/steiner_network_oracle.hpp"


namespace paal {
namespace ir {

template <typename Graph, typename CostMap, typename Restrictions>
class SteinerNetwork : public IRComponents < RowGenerationSolveLP < SteinerNetworkOracle < Graph, Restrictions> >,
                                               RoundConditionEquals<0> > {
public:
    typedef SteinerNetworkOracle < Graph, Restrictions > Oracle;
    typedef IRComponents < RowGenerationSolveLP < Oracle >,
                           RoundConditionEquals<0> > base;
  
    SteinerNetwork(const Graph & g, const CostMap & costMap, const Restrictions & restrictions) :
            base(RowGenerationSolveLP<Oracle>(m_separationOracle)),
            m_g(g), m_costMap(costMap), m_restrictions(restrictions),
            m_compare(EPSILON),
            m_separationOracle(m_g, m_restrictions, m_edgeMap, m_resultNetwork, m_compare) { }
    
    SteinerNetwork(SteinerNetwork && other) :
            base(RowGenerationSolveLP<Oracle>(m_separationOracle)),
            m_g(std::move(other.m_g)), m_costMap(std::move(other.m_costMap)), 
            m_restrictions(std::move(other.m_restrictions)),
            m_compare(std::move(other.m_compare)),
            m_separationOracle(m_g, m_restrictions, m_edgeMap, m_resultNetwork, m_compare) { }
    
    SteinerNetwork(const SteinerNetwork & other) :
            base(RowGenerationSolveLP<Oracle>(m_separationOracle)),
            m_g(other.m_g), m_costMap(other.m_costMap), 
            m_restrictions(other.m_restrictions),
            m_compare(other.m_compare),
            m_separationOracle(m_g, m_restrictions, m_edgeMap, m_resultNetwork, m_compare) { }
                           
    typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;
    typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
    
    typedef typename Oracle::ResultNetwork ResultNetwork;
    typedef typename Oracle::EdgeMap EdgeMap;
    typedef std::map<Edge, ColId> OriginalEdgeMap;
    typedef utils::Compare<double> Compare;

    template <typename LP>
    void init(LP & lp) {
        lp.setLPName("steiner network");
        lp.setMinObjFun();
        addVariables(lp);
        lp.loadMatrix();
        assert(m_separationOracle.checkIfSolutionExists());
    }

    template <typename LP>
    std::pair<bool, double> roundCondition(const LP & lp, ColId id) {
        auto ret = base::roundCondition(lp, id);
        if(ret.first) {
            assert(m_compare.e(ret.second, 0));
            //removing edge
            m_edgeMap.right.erase(id);
            return ret;
        } else {
            ret = m_roundHalf(lp, id);
            if(ret.first) {
                auto i = m_edgeMap.right.find(id);
                assert(i != m_edgeMap.right.end());

                m_resultNetwork.insert(i->second);
            }
            return ret;
        }
    }
    
    template <typename GetSolution>
    ResultNetwork & getSolution(const GetSolution& sol) {
        return m_resultNetwork;
    }

private:
    
    //adding variables
    template <typename LP>
    void addVariables(LP & lp) {
        auto edges = boost::edges(m_g);
        
        for(Edge e : utils::make_range(edges.first, edges.second)) {
            ColId col =  lp.addColumn(m_costMap[e], DB, 0, 1);
            m_originalEdgeMap.insert(typename OriginalEdgeMap::value_type(e, col));
            m_edgeMap.insert(typename EdgeMap::value_type(e, col));
        }
    }

    const Graph & m_g;
    const CostMap & m_costMap;
    const Restrictions & m_restrictions;
    
    EdgeMap          m_edgeMap;
    OriginalEdgeMap m_originalEdgeMap;
    
    ResultNetwork    m_resultNetwork;
    
    Compare m_compare;
    Oracle m_separationOracle;
    RoundConditionGreaterThanHalf m_roundHalf;
    static const double EPSILON;
};

template <typename Graph, typename CostMap, typename Restrictions>
const double SteinerNetwork<Graph, CostMap, Restrictions>::EPSILON = 1e-10;

template <typename Graph, typename CostMap, typename Restrictions>
SteinerNetwork<Graph, CostMap, Restrictions>
make_SteinerNetwork(const Graph & g, const CostMap & costMap, const Restrictions & rest) {
    return SteinerNetwork<Graph, CostMap, Restrictions>(g, costMap, rest);
}

} //ir
} //paal
#endif /* STEINER_NETWORK_HPP */
