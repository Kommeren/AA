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

namespace {
struct SteinerNetworkCompareTraits {
    static const double EPSILON;
};

const double SteinerNetworkCompareTraits::EPSILON = 1e-10;
}


template <typename Graph, typename CostMap, typename ResultNetworkSet>
class SteinerNetwork {
public:
  
    SteinerNetwork(const Graph & g, const CostMap & costMap, ResultNetworkSet & resultNetwork) :
            m_g(g), m_costMap(costMap), 
            m_resultNetwork(resultNetwork),
            m_compare(SteinerNetworkCompareTraits::EPSILON) {
            }
    
    SteinerNetwork(SteinerNetwork && other) :
            m_g(other.m_g), m_costMap(other.m_costMap), 
            m_resultNetwork(other.m_resultNetwork),
            m_compare(std::move(other.m_compare)) {}
    
    SteinerNetwork(const SteinerNetwork & other) :
            m_g(other.m_g), m_costMap(other.m_costMap), 
            m_resultNetwork(other.m_resultNetwork),
            m_compare(other.m_compare) {}
                           
    typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;
    typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
    
    typedef boost::bimap<Edge, ColId> EdgeMap;
    typedef std::vector<Vertex> VertexList;

    typedef std::map<Edge, ColId> OriginalEdgeMap;
    typedef utils::Compare<double> Compare;

    const EdgeMap & getEdgeMap() const  {
        return m_edgeMap;
    }

    const Graph & getGraph() {
        return m_g;
    }

    decltype(std::declval<CostMap>()[std::declval<Edge>()])
    getCost(Edge e) {
        return m_costMap[e];
    }

    void bindEdgeToCol(Edge e, ColId col) {
        m_originalEdgeMap.insert(typename OriginalEdgeMap::value_type(e, col));
        m_edgeMap.insert(typename EdgeMap::value_type(e, col));
    }

    void removeColumn(ColId colId) {        
        auto ret = m_edgeMap.right.erase(colId);
        assert(ret == 1);
    }
    
    void addColumnToSolution(ColId colId) {
        m_resultNetwork.insert(colToEdge(colId));
    }

    Compare getCompare() const {
        return m_compare;
    }

private:
    
    Edge colToEdge(ColId col) {
        auto i = m_edgeMap.right.find(col);
        assert(i != m_edgeMap.right.end());
        return i->second;
    }

    const Graph & m_g;
    const CostMap & m_costMap;
    ResultNetworkSet  &   m_resultNetwork;
    
    EdgeMap          m_edgeMap;
    OriginalEdgeMap m_originalEdgeMap;
    
    Compare m_compare;
};

template <typename Graph, typename CostMap, typename ResultNetworkSet>
SteinerNetwork<Graph, CostMap, ResultNetworkSet>
make_SteinerNetwork(const Graph & g, const CostMap & costMap, ResultNetworkSet & resultNetwork) {
    return SteinerNetwork<Graph, CostMap, ResultNetworkSet>(g, costMap, resultNetwork);
}

class SteinerNetworkInit {
public:
    template <typename Solution, typename LP>
    void operator()(Solution & solution, LP & lp) {
        lp.setLPName("steiner network");
        lp.setMinObjFun();
        addVariables(solution, lp);
        lp.loadMatrix();
        //TODO put it somwhere
        //assert(solution.checkIfSolutionExists());
    }
private:
    //adding variables
    template <typename Solution, typename LP>
    void addVariables(Solution & solution, LP & lp) {
        auto edges = boost::edges(solution.getGraph());
        
        for(auto e : utils::make_range(edges)) {
            ColId col = lp.addColumn(solution.getCost(e), DB, 0, 1);
            solution.bindEdgeToCol(e, col);
        }
    }
};


struct SteinerNetworkRoundCondition {
    SteinerNetworkRoundCondition(double epsilon = SteinerNetworkCompareTraits::EPSILON) :
        m_roundHalf(epsilon), m_roundZero(epsilon) {}

    template <typename Solution, typename LP>
    boost::optional<double> operator()(Solution & sol, const LP & lp, ColId colId) {
        auto ret = m_roundZero(sol, lp, colId);
        if(ret) {
            //removing edge
            sol.removeColumn(colId);
            return ret;
        } else {
            ret = m_roundHalf(sol, lp, colId);
            if(ret) {
                sol.addColumnToSolution(colId);
            }
            return ret;
        }
    }
private:
    RoundConditionGreaterThanHalf m_roundHalf;
    RoundConditionEquals<0>       m_roundZero;
};

template <
         typename Graph,
         typename Restrictions,
         typename ResultNetworkSet,
         typename SolveLPToExtremePoint = RowGenerationSolveLP < SteinerNetworkOracle < Graph, Restrictions, ResultNetworkSet> >, 
         typename RoundCondition = SteinerNetworkRoundCondition, 
         typename RelaxContition = utils::ReturnFalseFunctor, 
         typename Init = SteinerNetworkInit,
         typename SetSolution = utils::DoNothingFunctor>
             using  SteinerNetworkIRComponents = IRComponents<SolveLPToExtremePoint, RoundCondition, RelaxContition, Init, SetSolution>;


template <typename Graph, typename CostMap, typename ResultNetworkSet, typename IRComponents, typename Visitor = TrivialVisitor>
void steiner_network_iterative_rounding(const Graph & g, const CostMap & cost, ResultNetworkSet & result, IRComponents comps, Visitor vis = Visitor()) {
    auto steiner = paal::ir::make_SteinerNetwork(g, cost, result);
    paal::ir::solve_iterative_rounding(steiner, std::move(comps), std::move(vis));
}



} //ir
} //paal
#endif /* STEINER_NETWORK_HPP */
