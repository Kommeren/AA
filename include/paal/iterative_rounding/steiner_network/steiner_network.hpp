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
            m_solutionGenerated(false), m_separationOracle(m_g, m_restrictions, m_edgeMap, m_compare) { }
                           
    typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;
    typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
    
    typedef std::map<Edge, bool> ResultNetwork;
    typedef typename Oracle::EdgeMap EdgeMap;
    typedef std::map<Edge, ColId> OriginalEdgeMap;
    typedef std::vector<Edge> EdgeList;
    typedef std::vector<Vertex> VertexList;
    
    typedef utils::Compare<double> Compare;
    

    template <typename LP>
    void init(LP & lp) {
        lp.setLPName("steiner network");
        lp.setMinObjFun(); 
        
        addVariables(lp);
        lp.loadMatrix();
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
            return m_roundHalf(lp, id);
        }
    }
    
    template <typename GetSolution>
    ResultNetwork & getSolution(const GetSolution& sol) {
        if (!m_solutionGenerated) {
            generateSolution(sol);
        }
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
    
    template <typename GetSolution>
    void generateSolution(const GetSolution & sol) {
        for (auto const & edgeCol : m_originalEdgeMap ) {
            if(m_compare.e(sol(edgeCol.second), 1)) {
                m_resultNetwork.insert();
            }
        }
        m_solutionGenerated = true;
    }

    const Graph & m_g;
    const CostMap & m_costMap;
    const Restrictions & m_restrictions;
    
    EdgeMap          m_edgeMap;
    OriginalEdgeMap m_originalEdgeMap;
    
    bool            m_solutionGenerated;
    ResultNetwork    m_resultNetwork;
    
    Compare m_compare;
    Oracle m_separationOracle;
    RoundConditionGreaterThanHalf m_roundHalf;
};

template <typename Graph, typename CostMap, typename Restrictions>
SteinerNetwork<Graph, CostMap, Restrictions>
make_SteinerNetwork(const Graph & g, const CostMap & costMap, const Restrictions & rest) {
    return SteinerNetwork<Graph, CostMap, Restrictions>(g, costMap, rest);
}

} //ir
} //paal
#endif /* STEINER_NETWORK_HPP */
