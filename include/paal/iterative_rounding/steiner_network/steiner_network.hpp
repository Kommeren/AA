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
#include "paal/lp/lp_row_generation.hpp"
#include "paal/iterative_rounding/steiner_network/steiner_network_oracle.hpp"


namespace paal {
namespace ir {

namespace {
struct SteinerNetworkCompareTraits {
    static const double EPSILON;
};

const double SteinerNetworkCompareTraits::EPSILON = 1e-10;
}


/**
 * @class SteinerNetwork
 * @brief The class for solving the Steiner Network problem using Iterative Rounding.
 *
 * @tparam Graph input graph
 * @tparam Restrictions connectivity restrictions for vertex pairs
 * @tparam CostMap map of edge costs
 * @tparam ResultNetworkSet
 */
template <typename Graph, typename Restrictions, typename CostMap, typename ResultNetworkSet>
class SteinerNetwork {
public:
  
    SteinerNetwork(const Graph & g, const Restrictions & restrictions,
                   const CostMap & costMap, ResultNetworkSet & resultNetwork) :
            m_g(g), m_restrictions(restrictions),
            m_costMap(costMap), m_resultNetwork(resultNetwork),
            m_compare(SteinerNetworkCompareTraits::EPSILON) {}
    
    SteinerNetwork(SteinerNetwork && other) :
            m_g(other.m_g), m_restrictions(other.m_restrictions),
            m_costMap(other.m_costMap), m_resultNetwork(other.m_resultNetwork),
            m_compare(std::move(other.m_compare)) {}
    
    SteinerNetwork(const SteinerNetwork & other) :
            m_g(other.m_g), m_restrictions(other.m_restrictions),
            m_costMap(other.m_costMap), m_resultNetwork(other.m_resultNetwork),
            m_compare(other.m_compare) {}
                           
    typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;
    typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
    
    typedef std::unordered_map<lp::ColId, Edge> EdgeMap;
    typedef std::vector<Vertex> VertexList;

    typedef utils::Compare<double> Compare;

    typedef boost::optional<std::string> ErrorMessage;

    /**
     * Checks if the connectivity restrictions can be fulfilled.
     */
    ErrorMessage checkInputValidity() {
        auto oracle = make_SteinerNetworkSeparationOracle(m_g, m_restrictions, m_resultNetwork);
        if (!oracle.checkIfSolutionExists(*this)) {
            return ErrorMessage("A Steiner network satisfying the restrictions does not exist.");
        }
        else {
            return ErrorMessage();
        }
    }

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

    /**
     * Binds a graph edge to a LP column.
     */
    void bindEdgeToCol(Edge e, lp::ColId col) {
        m_edgeMap.insert(typename EdgeMap::value_type(col, e));
    }

    /**
     * Removes an LP column and the graph edge corresponding to it.
     */
    void removeColumn(lp::ColId colId) {        
        auto ret = m_edgeMap.erase(colId);
        assert(ret == 1);
    }

    /**
     * Adds an edge corresponding to the given column to the result set.
     */
    void addColumnToSolution(lp::ColId colId) {
        m_resultNetwork.insert(colToEdge(colId));
    }

    Compare getCompare() const {
        return m_compare;
    }

private:
    
    Edge colToEdge(lp::ColId col) {
        auto i = m_edgeMap.find(col);
        assert(i != m_edgeMap.end());
        return i->second;
    }

    const Graph & m_g;
    const Restrictions & m_restrictions;
    const CostMap & m_costMap;
    ResultNetworkSet  &   m_resultNetwork;
    
    EdgeMap m_edgeMap;
    
    Compare m_compare;
};

/**
 * @brief Creates a SteinerNetwork object.
 *
 * @tparam Graph
 * @tparam Restrictions
 * @tparam CostMap
 * @tparam ResultNetworkSet
 * @param g
 * @param restrictions
 * @param costMap
 * @param resultNetwork
 *
 * @return SteinerNetwork object
 */
template <typename Graph, typename Restrictions, typename CostMap, typename ResultNetworkSet>
SteinerNetwork<Graph, Restrictions, CostMap, ResultNetworkSet>
make_SteinerNetwork(const Graph & g, const Restrictions & restrictions,
                    const CostMap & costMap, ResultNetworkSet & resultNetwork) {
    return SteinerNetwork<Graph, Restrictions, CostMap, ResultNetworkSet>(
                                                g, restrictions, costMap, resultNetwork);
}

/**
 * Initialization of the IR Steiner Network algorithm.
 */
class SteinerNetworkInit {
public:
    /**
     * Initializes the LP: variables for edges.
     */
    template <typename Problem, typename LP>
    void operator()(Problem & problem, LP & lp) {
        lp.setLPName("steiner network");
        lp.setMinObjFun();
        addVariables(problem, lp);
        lp.loadMatrix();
    }

private:
    //adding variables
    template <typename Problem, typename LP>
    void addVariables(Problem & problem, LP & lp) {
        for(auto e : boost::make_iterator_range(edges(problem.getGraph()))) {
            lp::ColId col = lp.addColumn(problem.getCost(e), lp::DB, 0, 1);
            problem.bindEdgeToCol(e, col);
        }
    }
};


/**
 * Round Condition of the IR Steiner Network algorithm.
 */
struct SteinerNetworkRoundCondition {
    SteinerNetworkRoundCondition(double epsilon = SteinerNetworkCompareTraits::EPSILON) :
        m_roundHalf(epsilon), m_roundZero(epsilon) {}

    /**
     * Checks if a given column of the LP can be rounded to 0 or if it is greater
     * then 1/2.
     * If the column is rounded to 0, the corresponding edge is removed from the graph.
     * If the column is greater than 1/2, it is rounded to 1 and the corresponding edge
     * is added to the solution.
     */
    template <typename Problem, typename LP>
    boost::optional<double> operator()(Problem & problem, const LP & lp, lp::ColId colId) {
        auto ret = m_roundZero(problem, lp, colId);
        if(ret) {
            //removing edge
            problem.removeColumn(colId);
            return ret;
        } else {
            ret = m_roundHalf(problem, lp, colId);
            if(ret) {
                problem.addColumnToSolution(colId);
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
         typename SolveLPToExtremePoint = lp::RowGenerationSolveLP < SteinerNetworkOracle < Graph, Restrictions, ResultNetworkSet> >, 
         typename RoundCondition = SteinerNetworkRoundCondition, 
         typename RelaxContition = utils::ReturnFalseFunctor, 
         typename Init = SteinerNetworkInit,
         typename SetSolution = utils::SkipFunctor>
             using  SteinerNetworkIRComponents = IRComponents<SolveLPToExtremePoint, RoundCondition, RelaxContition, Init, SetSolution>;


/**
 * @brief Solves the Steiner Network problem using Iterative Rounding.
 *
 * @tparam Graph
 * @tparam Restrictions
 * @tparam CostMap
 * @tparam ResultNetworkSet
 * @tparam IRComponents
 * @tparam Visitor
 * @param g
 * @param restrictions
 * @param cost
 * @param result
 * @param components
 * @param visitor
 *
 * @return solution status
 */
template <typename Graph, typename Restrictions, typename CostMap, typename ResultNetworkSet,
          typename IRComponents, typename Visitor = TrivialVisitor>
lp::ProblemType steiner_network_iterative_rounding(const Graph & g, const Restrictions & restrictions,
        const CostMap & cost, ResultNetworkSet & result, IRComponents components, Visitor visitor = Visitor()) {
    auto steiner = paal::ir::make_SteinerNetwork(g, restrictions, cost, result);
    return paal::ir::solve_iterative_rounding(steiner, std::move(components), std::move(visitor));
}



} //ir
} //paal
#endif /* STEINER_NETWORK_HPP */
