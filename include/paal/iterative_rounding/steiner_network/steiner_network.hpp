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
#include <boost/graph/named_function_params.hpp>

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
 * @tparam ResultNetworkOutputIterator
 */
template <typename Graph, typename Restrictions, typename CostMap, typename ResultNetworkOutputIterator>
class SteinerNetwork {
public:

    /**
     * Constructor.
     */
    SteinerNetwork(const Graph & g, const Restrictions & restrictions,
                   CostMap costMap, ResultNetworkOutputIterator resultNetwork) :
            m_g(g), m_restrictions(restrictions),
            m_costMap(costMap), m_resultNetwork(resultNetwork),
            m_compare(SteinerNetworkCompareTraits::EPSILON) {}

    typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;
    typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;

    typedef std::unordered_map<lp::ColId, Edge> EdgeMap;
    typedef std::vector<Edge> EdgeList;
    typedef std::vector<Vertex> VertexList;

    typedef utils::Compare<double> Compare;

    typedef boost::optional<std::string> ErrorMessage;

    /**
     * Checks if the connectivity restrictions can be fulfilled.
     */
    ErrorMessage checkInputValidity() {
        auto oracle = make_SteinerNetworkSeparationOracle(m_g, m_restrictions);
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
        auto e = colToEdge(colId);
        *m_resultNetwork = e;
        ++m_resultNetwork;
        m_resultList.push_back(e);
    }

    /**
     * Returns the list of edges that are already added to the solution.
     */
    const EdgeList & getEdgesInSolution() const {
        return m_resultList;
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
    CostMap m_costMap;
    ResultNetworkOutputIterator m_resultNetwork;

    EdgeMap m_edgeMap;
    EdgeList m_resultList;

    Compare m_compare;
};

namespace detail {
/**
 * @brief Creates a SteinerNetwork object. Non-named version.
 *
 * @tparam Graph
 * @tparam Restrictions
 * @tparam CostMap
 * @tparam ResultNetworkOutputIterator
 * @param g
 * @param restrictions
 * @param costMap
 * @param resultNetwork
 *
 * @return SteinerNetwork object
 */
template <typename Graph, typename Restrictions, typename CostMap, typename ResultNetworkOutputIterator>
SteinerNetwork<Graph, Restrictions, CostMap, ResultNetworkOutputIterator>
make_SteinerNetwork(const Graph & g, const Restrictions & restrictions,
                    CostMap costMap, ResultNetworkOutputIterator resultNetwork) {
    return SteinerNetwork<Graph, Restrictions, CostMap, ResultNetworkOutputIterator>(
                                    g, restrictions, costMap, resultNetwork);
}
} // detail

/**
 * Creates a SteinerNetwork object. Named version.
 * The returned object can be used to check input validity or to get a lower bound on the
 * optimal solution cost.
 *
 * @tparam Graph
 * @tparam Restrictions
 * @tparam ResultNetworkOutputIterator
 * @tparam P
 * @tparam T
 * @tparam R
 * @param g
 * @param restrictions
 * @param params
 * @param resultNetwork
 *
 * @return SteinerNetwork object
 */
template <typename Graph, typename Restrictions, typename ResultNetworkOutputIterator,
            typename P, typename T, typename R>
auto
make_SteinerNetwork(const Graph & g, const Restrictions & restrictions,
                    const boost::bgl_named_params<P, T, R>& params,
                    ResultNetworkOutputIterator resultNetwork) ->
       SteinerNetwork<Graph, Restrictions,
            decltype(choose_const_pmap(get_param(params, boost::edge_weight), g, boost::edge_weight)),
            ResultNetworkOutputIterator> {
    return detail::make_SteinerNetwork(g, restrictions,
                choose_const_pmap(get_param(params, boost::edge_weight), g, boost::edge_weight),
                resultNetwork);
}

/**
 * Creates a SteinerNetwork object. All default parameters.
 * The returned object can be used to check input validity or to get a lower bound on the
 * optimal solution cost.
 *
 * @tparam Graph
 * @tparam Restrictions
 * @tparam ResultNetworkOutputIterator
 * @param g
 * @param restrictions
 * @param resultNetwork
 *
 * @return SteinerNetwork object
 */
template <typename Graph, typename Restrictions, typename ResultNetworkOutputIterator>
auto
make_SteinerNetwork(const Graph & g, const Restrictions & restrictions,
                    ResultNetworkOutputIterator resultNetwork) ->
        decltype(make_SteinerNetwork(g, restrictions, boost::no_named_parameters(), resultNetwork)) {
    return make_SteinerNetwork(g, restrictions, boost::no_named_parameters(), resultNetwork);
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
                problem.removeColumn(colId);
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
         typename SolveLPToExtremePoint = lp::RowGenerationSolveLP<SteinerNetworkOracle<Graph, Restrictions>>,
         typename RoundCondition = SteinerNetworkRoundCondition,
         typename RelaxContition = utils::ReturnFalseFunctor,
         typename Init = SteinerNetworkInit,
         typename SetSolution = utils::SkipFunctor>
             using  SteinerNetworkIRComponents = IRComponents<SolveLPToExtremePoint, RoundCondition, RelaxContition, Init, SetSolution>;


namespace detail {
/**
 * @brief Solves the Steiner Network problem using Iterative Rounding. Non-named version.
 *
 * @tparam Graph
 * @tparam Restrictions
 * @tparam CostMap
 * @tparam ResultNetworkOutputIterator
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
template <typename Graph, typename Restrictions, typename CostMap, typename ResultNetworkOutputIterator,
          typename IRComponents, typename Visitor = TrivialVisitor>
lp::ProblemType steiner_network_iterative_rounding(const Graph & g, const Restrictions & restrictions,
        CostMap cost, ResultNetworkOutputIterator result, IRComponents components, Visitor visitor = Visitor()) {
    auto steiner = make_SteinerNetwork(g, restrictions, cost, result);
    return solve_iterative_rounding(steiner, std::move(components), std::move(visitor));
}
} // detail

/**
 * @brief Solves the Steiner Network problem using Iterative Rounding. Named version.
 *
 * @tparam Graph
 * @tparam Restrictions
 * @tparam ResultNetworkOutputIterator
 * @tparam IRComponents
 * @tparam Visitor
 * @tparam P
 * @tparam T
 * @tparam R
 * @param g
 * @param restrictions
 * @param params
 * @param result
 * @param components
 * @param visitor
 *
 * @return solution status
 */
template <typename Graph, typename Restrictions, typename ResultNetworkOutputIterator,
          typename IRComponents, typename Visitor = TrivialVisitor,
          typename P, typename T, typename R>
lp::ProblemType steiner_network_iterative_rounding(const Graph & g, const Restrictions & restrictions,
        const boost::bgl_named_params<P, T, R> & params,
        ResultNetworkOutputIterator result, IRComponents components, Visitor visitor = Visitor()) {
    return detail::steiner_network_iterative_rounding(g, restrictions,
                choose_const_pmap(get_param(params, boost::edge_weight), g, boost::edge_weight),
                std::move(result), std::move(components), std::move(visitor));
}

/**
 * @brief Solves the Steiner Network problem using Iterative Rounding. All default parameters.
 *
 * @tparam Graph
 * @tparam Restrictions
 * @tparam ResultNetworkOutputIterator
 * @tparam IRComponents
 * @tparam Visitor
 * @param g
 * @param restrictions
 * @param result
 * @param components
 * @param visitor
 *
 * @return solution status
 */
template <typename Graph, typename Restrictions, typename ResultNetworkOutputIterator,
          typename IRComponents, typename Visitor = TrivialVisitor>
lp::ProblemType steiner_network_iterative_rounding(const Graph & g, const Restrictions & restrictions,
        ResultNetworkOutputIterator result, IRComponents components, Visitor visitor = Visitor()) {
    return steiner_network_iterative_rounding(g, restrictions, boost::no_named_parameters(),
                std::move(result), std::move(components), std::move(visitor));
}


} //ir
} //paal
#endif /* STEINER_NETWORK_HPP */
