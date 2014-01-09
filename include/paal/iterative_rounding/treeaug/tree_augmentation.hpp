/**
 * @file tree_augmentation.hpp
 * @brief
 * @author Attila Bernath, Piotr Smulewicz, Piotr Wygocki, Piotr Godlewski
 * @version 1.0
 * @date 2013-06-20
 */
#ifndef TREE_AUG_HPP
#define TREE_AUG_HPP

#include <utility>                   // for std::pair
#include <algorithm>                 // for std::for_each

#include <boost/bimap.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/named_function_params.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/stoer_wagner_min_cut.hpp>
#include <boost/range/distance.hpp>

#include "paal/iterative_rounding/iterative_rounding.hpp"
#include "paal/iterative_rounding/ir_components.hpp"
#include "paal/utils/bgl_edge_hash.hpp"

namespace paal {
namespace ir {

/// This function returns the number of edges in a graph. The
/// reason this function was written is that BGL's num_edges()
/// function does not work properly for filtered_graph.  See
/// http://www.boost.org/doc/libs/1_55_0/libs/graph/doc/filtered_graph.html#2
template <class EdgeListGraph>
int filtered_num_edges(const EdgeListGraph & G)
{
    return boost::distance(edges(G));
}


/// A class that translates bool map on the edges to a filter, which can be used with
/// boost::filtered_graph. That is, we translate operator[] to
/// operator().  We do a little more: we will also need the negated
/// version of the map (for the graph of the non-tree edges).
template <typename EdgeBoolMap>
struct BoolMapToTreeFilter {
    BoolMapToTreeFilter() { }

    BoolMapToTreeFilter(EdgeBoolMap  m) : ebmap(m) { }

    template <typename Edge>
    bool operator()(const Edge & e) const {
            return get(ebmap, e);
    }

    EdgeBoolMap ebmap;
};

template <typename EdgeBoolMap>
struct BoolMapToNonTreeFilter {
    BoolMapToNonTreeFilter() { }

    BoolMapToNonTreeFilter(EdgeBoolMap  m) : ebmap(m) { }

    template <typename Edge>
    bool operator()(const Edge & e) const {
            return !get(ebmap, e);
    }

    EdgeBoolMap ebmap;
};


/// A boost graph map that returns a constant integer value.
template <typename KeyType, int num>
class ConstIntMap
    : public boost::put_get_helper<int, ConstIntMap<KeyType, num>>
{
public:
    typedef boost::readable_property_map_tag category;
    typedef int value_type;
    typedef int reference;
    typedef KeyType key_type;
    reference operator[](KeyType e) const { return num; }
};


/**
 * Round Condition of the IR Tree Augmentation algorithm.
 */
struct TARoundCondition {
    /**
     * Rounds a column up if it is at least half in the
     * current solution. If the column is rounded, the
     * corresponding edge is added to the result.
     */
    template <typename Problem, typename LP>
    boost::optional<double> operator()(Problem & problem, LP & lp, lp::ColId col) {
        auto res = m_roundHalf(problem, lp, col);
        if (res) {
            problem.addToSolution(col);
        }
        return res;
    }
private:
    RoundConditionGreaterThanHalf m_roundHalf;
};


/**
 * Relax Condition of the IR Tree Augmentation algorithm.
 */
struct TARelaxCondition {
    /**
     * A condition belonging to a tree edge \c t can
     * be relaxed if some link in the fundamental cut
     * belonging to \c t is chosen in the solution.
     *
     * In fact we don't even need to relax, since
     * these constraints became trivial after
     * rounding. So returning always false should be
     * fine, too.
     */
    template <typename Problem, typename LP>
    bool operator()(Problem & problem, const LP & lp, lp::RowId row) {
        for (auto e : problem.getCoveredBy(problem.rowToEdge(row))) {
            if (problem.isInSolution(e))
                return true;
        }
        return false;
    }
};

/**
 * Initialization of the IR Tree Augmentation algorithm.
 */
class TAInit {
public:
    /**
     * Initialize the cut LP.
     */
    template <typename Problem, typename LP>
    void operator()(Problem & problem, LP & lp) {
        problem.init();
        lp.setLPName("Tree augmentation");
        lp.setMinObjFun();

        addVariables(problem, lp);

        addCutConstraints(problem, lp);

        lp.loadMatrix();
    }

private:
    /**
     * Adds a variable to the LP for each link in the input graph.
     * Binds the LP columns to graph edges.
     */
    template <typename Problem, typename LP>
    void addVariables(Problem & problem, LP & lp) {
        int eIdx{0};

        for (auto e : boost::make_iterator_range(edges(problem.getLinksGraph()))) {
            std::string colName = getEdgeName(eIdx);
            lp::ColId colIdx = lp.addColumn(problem.getCost(e), lp::LO, 0, -1, colName);
            problem.bindEdgeToCol(e, colIdx);
            ++eIdx;
        }
    }

    /**
     * Adds a cut constraint to the LP for each edge in the input graph
     * and binds edges to LP rows.
     */
    template <typename Problem, typename LP>
    void addCutConstraints(Problem & problem, LP & lp) {
        int dbIndex = 0;

        for (auto e : boost::make_iterator_range(edges(problem.getTreeGraph()))) {
            lp::RowId rowIdx = lp.addRow(lp::LO, 1, -1, getRowName(dbIndex));
            problem.bindEdgeToRow(e, rowIdx);

            for (auto pe : problem.getCoveredBy(e)) {
                lp.addConstraintCoef(rowIdx, problem.edgeToCol(pe));
            }
            ++dbIndex;
        }
    }

    std::string getEdgeName(int eIdx) const {
        return std::to_string(eIdx);
    }

    std::string getCutConstrPrefix() const {
        return "cutConstraint";
    }

    std::string getRowName(int dbIdx) const {
        return getCutConstrPrefix() + std::to_string(dbIdx);
    }
};

template <
    typename SolveLPToExtremePoint = DefaultSolveLPToExtremePoint,
    typename RoundCondition = TARoundCondition,
    typename RelaxContition = TARelaxCondition,
    typename Init = TAInit,
    typename SetSolution = utils::SkipFunctor>
        using  TAComponents = IRComponents<SolveLPToExtremePoint, RoundCondition, RelaxContition, Init, SetSolution>;

/**
 * @brief This is Jain's iterative rounding
 * 2-approximation algorithm for the Generalised Steiner Network
 * Problem, specialized for the Tree Augmentation Problem.
 *
 * The Tree Augmentation Problem is the following. Given a
 * 2-edge connected graph, in which a spanning tree is
 * designated. The non-tree edges are also called links. The
 * links have non-negative costs. The problem is to find a
 * minimum cost subset of links which, together with the
 * tree-edges give a 2-edge-connected graph.
 *
 * @tparam Graph the graph type used
 * @tparam TreeMap it is assumed to be a bool map on the edges of a graph of type Graph. It is used for designating a spanning tree in the graph.
 * @tparam CostMap type for the costs of the links.
 * @tparam EdgeSetOutputIterator type for the result edge set.
 */
template <typename Graph, typename TreeMap, typename CostMap, class EdgeSetOutputIterator>
class TreeAug {
public:

    typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;
    typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
    typedef double CostValue;

    typedef boost::filtered_graph<Graph, BoolMapToTreeFilter<TreeMap> > TreeGraph;
    typedef boost::filtered_graph<Graph, BoolMapToNonTreeFilter<TreeMap> > NonTreeGraph;

    typedef std::vector<Edge> EdgeList;
    typedef std::unordered_map<Edge, EdgeList, EdgeHash<Graph>> CoverMap;

    //cross reference between links and columns
    typedef boost::bimap<Edge, lp::ColId> EdgeToColId;
    typedef std::unordered_map<lp::RowId, Edge> RowIdToEdge;

    typedef boost::optional<std::string> ErrorMessage;

    /**
     * Constructor.
     *
     * @param g  the graph to work with
     * @param treeMap designate a spanning tree in \c g
     * @param costMap costs of the links (=non-tree edges). The costs assigned to tree edges are not used.
     * @param solution result set of edges output iterator
     */
    TreeAug(const Graph & g, TreeMap treeMap, CostMap costMap, EdgeSetOutputIterator solution) :
        m_g(g), m_treeMap(treeMap), m_costMap(costMap),
        m_solution(solution),
        m_tree(m_g, BoolMapToTreeFilter<TreeMap>(m_treeMap)),
        m_ntree(m_g, BoolMapToNonTreeFilter<TreeMap>(m_treeMap)),
        m_solCost(0)
    {}

    /// Checks validity of the input
    ErrorMessage checkInputValidity() {
        // Num of edges == num of nodes-1 in the tree?
        int nV = num_vertices(m_g);
        int nE = filtered_num_edges(m_tree);

        if (nE != nV - 1) {
            return ErrorMessage(
                        "Incorrect number of edges in the spanning tree. "
                        + std::string("Should be ") + std::to_string(nV - 1)
                        + ", but it is " + std::to_string(nE) + "."
                    );
        }

        // Is the tree connected?
        std::vector<int> component(num_vertices(m_g));
        int num = boost::connected_components(m_tree, &component[0]);

        if (num > 1) {
            return ErrorMessage(
                        "The spanning tree is not connected."
                    );
        }

        // Is the graph 2-edge-connected?
        ConstIntMap<Edge, 1> const1EdgeMap;
        // TODO This stoer-wagner algorithm is unnecessarily slow for some reason
        int minCut = boost::stoer_wagner_min_cut(m_g, const1EdgeMap);
        if (minCut < 2) {
            return ErrorMessage(
                        "The graph is not 2-edge-connected."
                    );
        }

        return ErrorMessage();
    }

    const NonTreeGraph & getLinksGraph() const {
        return m_ntree;
    }

    const TreeGraph & getTreeGraph() const {
        return m_tree;
    }

    decltype(get(std::declval<CostMap>(), std::declval<Edge>()))
    getCost(Edge e) {
        return get(m_costMap, e);
    }

    /**
     * Adds an edge corresponding to the given LP column to the result set.
     */
    void addToSolution(lp::ColId col) {
        *m_solution = m_edgeToColId.right.at(col);
        ++m_solution;
        m_solCost += m_costMap[m_edgeToColId.right.at(col)];
        m_edgeToColId.right.erase(col);
    }

    /**
     * Binds a graph edge to a LP column.
     */
    void bindEdgeToCol(Edge e, lp::ColId col) {
        auto tmp = m_edgeToColId.insert(typename EdgeToColId::value_type(e, col));
        assert(tmp.second);
    }

    /**
     * Binds a graph edge to a LP row.
     */
    void bindEdgeToRow(Edge e, lp::RowId row) {
        auto tmp = m_rowIdToEdge.insert(typename RowIdToEdge::value_type(row, e));
        assert(tmp.second);
    }

    /**
     * Initializes the necessary data structures.
     */
    void init() {
        //We need to fill very useful auxiliary data structures:
        //\c m_coveredBy - containing lists of
        //edges. For a tree edge \c t the list \c m_coveredBy[t]
        //contains the list of links covering \c t.

        std::vector<Edge> pred(num_vertices(m_g));
        std::set<Vertex> seen;
        for (auto u : boost::make_iterator_range(vertices(m_g))) {
            auto tmp = seen.insert(u);
            assert(tmp.second);
            boost::breadth_first_search(m_tree, u,
                                 visitor(make_bfs_visitor(record_edge_predecessors(&pred[0], boost::on_tree_edge()))));

            for (auto e : boost::make_iterator_range(out_edges(u, m_ntree))) {
                auto node = target(e, m_ntree);
                if (!seen.count(node)) {
                    while (node != u) {
                        m_coveredBy[pred[node]].push_back(e);
                        node = source(pred[node], m_tree);
                    }
                }
            }
        }
    }

    /**
     * Returns the edge corresponding to an LP row.
     */
    Edge rowToEdge(lp::RowId row) const {
        return m_rowIdToEdge.at(row);
    }

    /**
     * Returns the LP columnn corresponding to a graph edge.
     */
    lp::ColId edgeToCol(Edge e) const {
        return m_edgeToColId.left.at(e);
    }

    /**
     * Returns the list of links covering a given edge.
     */
    EdgeList & getCoveredBy(Edge e) {
        return m_coveredBy[e];
    }

    bool isInSolution(Edge e) const {
        return m_edgeToColId.left.find(e) == m_edgeToColId.left.end();
    }

    /**
     * Returns cost of the found solution.
     */
    CostValue getSolutionValue() const {
        return m_solCost;
    }

private:

    /// The input
    const Graph & m_g;
    TreeMap m_treeMap;
    CostMap m_costMap;

    /// Which links are chosen in the solution
    EdgeSetOutputIterator m_solution;

    /// Auxiliary data structures
    EdgeToColId m_edgeToColId;

    /// The spanning tree
    TreeGraph  m_tree;
    /// The non-tree (=set of links)
    NonTreeGraph m_ntree;
    /// Cost of the solution found
    CostValue m_solCost;
    /// Structures for the "m_coveredBy" relations
    CoverMap m_coveredBy;
    /// Reference between tree edges and row names
    RowIdToEdge m_rowIdToEdge;
};

namespace detail {
/**
 * @brief Creates a TreeAug object. Non-named parameters.
 *
 * @tparam Graph
 * @tparam TreeMap
 * @tparam CostMap
 * @tparam EdgeSetOutputIterator
 * @param g
 * @param treeMap
 * @param costMap
 * @param solution
 *
 * @return TreeAug object
 */
template <typename Graph, typename TreeMap, typename CostMap, typename EdgeSetOutputIterator>
TreeAug<Graph, TreeMap, CostMap, EdgeSetOutputIterator>
make_TreeAug(const Graph & g, TreeMap treeMap, CostMap costMap, EdgeSetOutputIterator solution) {
    return paal::ir::TreeAug<Graph, TreeMap, CostMap, EdgeSetOutputIterator>(g, treeMap, costMap, solution);
}

/**
 * @brief Solves the Tree Augmentation problem using Iterative Rounding. Non-named parameters.
 *
 * @tparam Graph
 * @tparam TreeMap
 * @tparam CostMap
 * @tparam EdgeSetOutputIterator
 * @tparam IRComponents
 * @tparam Visitor
 * @param g
 * @param treeMap
 * @param costMap
 * @param solution
 * @param components
 * @param visitor
 *
 * @return solution status
 */
template <typename Graph, typename TreeMap,
          typename CostMap, typename EdgeSetOutputIterator,
          typename IRComponents, typename Visitor = TrivialVisitor>
lp::ProblemType tree_augmentation_iterative_rounding(
        const Graph & g,
        TreeMap treeMap,
        CostMap costMap,
        EdgeSetOutputIterator solution,
        IRComponents components,
        Visitor visitor = Visitor()) {
    auto treeaug = make_TreeAug(g, treeMap, costMap, solution);
    return solve_iterative_rounding(treeaug, std::move(components), std::move(visitor));
}
} // detail

/**
 * Creates a TreeAug object. Named parameters.
 * The returned object can be used to check input validity or to get a lower bound on the
 * optimal solution cost.
 *
 * @tparam Graph
 * @tparam EdgeSetOutputIterator
 * @tparam P
 * @tparam T
 * @tparam R
 * @param g
 * @param params
 * @param solution
 *
 * @return TreeAug object
 */
template <typename Graph, typename EdgeSetOutputIterator, typename P, typename T, typename R>
auto
make_TreeAug(const Graph & g, const boost::bgl_named_params<P, T, R> & params, EdgeSetOutputIterator solution) ->
        TreeAug<Graph,
                decltype(choose_const_pmap(get_param(params, boost::edge_color), g, boost::edge_color)),
                decltype(choose_const_pmap(get_param(params, boost::edge_weight), g, boost::edge_weight)),
                EdgeSetOutputIterator> {
    return detail::make_TreeAug(g,
            choose_const_pmap(get_param(params, boost::edge_color), g, boost::edge_color),
            choose_const_pmap(get_param(params, boost::edge_weight), g, boost::edge_weight),
            solution);
}

/**
 * Creates a TreeAug object. All default parameters.
 * The returned object can be used to check input validity or to get a lower bound on the
 * optimal solution cost.
 *
 * @tparam Graph
 * @tparam EdgeSetOutputIterator
 * @param g
 * @param solution
 *
 * @return TreeAug object
 */
template <typename Graph, typename EdgeSetOutputIterator>
auto
make_TreeAug(const Graph & g, EdgeSetOutputIterator solution) ->
        decltype(make_TreeAug(g, boost::no_named_parameters(), solution)) {
    return make_TreeAug(g, boost::no_named_parameters(), solution);
}

/**
 * @brief Solves the Tree Augmentation problem using Iterative Rounding. Named parameters.
 *
 * @tparam Graph
 * @tparam EdgeSetOutputIterator
 * @tparam IRComponents
 * @tparam Visitor
 * @tparam P
 * @tparam T
 * @tparam R
 * @param g
 * @param params
 * @param solution
 * @param components
 * @param visitor
 *
 * @return solution status
 */
template <typename Graph, typename EdgeSetOutputIterator,
          typename IRComponents, typename Visitor = TrivialVisitor,
          typename P, typename T, typename R>
lp::ProblemType tree_augmentation_iterative_rounding(
        const Graph & g,
        const boost::bgl_named_params<P, T, R> & params,
        EdgeSetOutputIterator solution,
        IRComponents components,
        Visitor visitor = Visitor()) {
    return detail::tree_augmentation_iterative_rounding(g,
            choose_const_pmap(get_param(params, boost::edge_color), g, boost::edge_color),
            choose_const_pmap(get_param(params, boost::edge_weight), g, boost::edge_weight),
            std::move(solution), std::move(components), std::move(visitor));
}

/**
 * @brief Solves the Tree Augmentation problem using Iterative Rounding. All default parameters.
 *
 * @tparam Graph
 * @tparam EdgeSetOutputIterator
 * @tparam IRComponents
 * @tparam Visitor
 * @param g
 * @param solution
 * @param components
 * @param visitor
 *
 * @return solution status
 */
template <typename Graph, typename EdgeSetOutputIterator,
          typename IRComponents, typename Visitor = TrivialVisitor>
lp::ProblemType tree_augmentation_iterative_rounding(
        const Graph & g,
        EdgeSetOutputIterator solution,
        IRComponents components,
        Visitor visitor = Visitor()) {
    return tree_augmentation_iterative_rounding(g, boost::no_named_parameters(),
            std::move(solution), std::move(components), std::move(visitor));
}

} //ir
} //paal

#endif /* TREE_AUG_HPP */
