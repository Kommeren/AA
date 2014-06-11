/**
 * @file tree_augmentation.hpp
 * @brief
 * @author Attila Bernath, Piotr Smulewicz, Piotr Wygocki, Piotr Godlewski
 * @version 1.0
 * @date 2013-06-20
 */
#ifndef TREE_AUG_HPP
#define TREE_AUG_HPP


#include "paal/iterative_rounding/iterative_rounding.hpp"
#include "paal/iterative_rounding/ir_components.hpp"
#include "paal/utils/bgl_edge_hash.hpp"

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

#include <utility>                   // for std::pair
#include <algorithm>                 // for std::for_each

namespace paal {
namespace ir {

namespace detail {

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
struct bool_map_to_tree_filter {
    bool_map_to_tree_filter() { }

    bool_map_to_tree_filter(EdgeBoolMap  m) : ebmap(m) { }

    template <typename Edge>
    bool operator()(const Edge & e) const {
        return get(ebmap, e);
    }

    EdgeBoolMap ebmap;
};

template <typename EdgeBoolMap>
struct bool_map_to_non_tree_filter {
    bool_map_to_non_tree_filter() { }

    bool_map_to_non_tree_filter(EdgeBoolMap  m) : ebmap(m) { }

    template <typename Edge>
    bool operator()(const Edge & e) const {
        return !get(ebmap, e);
    }

    EdgeBoolMap ebmap;
};


/// A boost graph map that returns a constant integer value.
template <typename KeyType, int num>
class const_int_map
    : public boost::put_get_helper<int, const_int_map<KeyType, num>>
{
public:
    typedef boost::readable_property_map_tag category;
    typedef int value_type;
    typedef int reference;
    typedef KeyType key_type;
    reference operator[](KeyType e) const { return num; }
};

} // namespace detail

/**
 * Round Condition of the IR Tree Augmentation algorithm.
 */
struct ta_round_condition {
    /**
     * Rounds a column up if it is at least half in the
     * current solution. If the column is rounded, the
     * corresponding edge is added to the result.
     */
    template <typename Problem, typename LP>
    boost::optional<double> operator()(Problem & problem, LP & lp, lp::col_id col) {
        auto res = m_round_half(problem, lp, col);
        if (res) {
            problem.add_to_solution(col);
        }
        return res;
    }
private:
    round_condition_greater_than_half m_round_half;
};


/**
 * Relax Condition of the IR Tree Augmentation algorithm.
 */
struct ta_relax_condition {
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
    bool operator()(Problem & problem, const LP & lp, lp::row_id row) {
        for (auto e : problem.get_covered_by(problem.row_to_edge(row))) {
            if (problem.is_in_solution(e))
                return true;
        }
        return false;
    }
};

/**
 * Initialization of the IR Tree Augmentation algorithm.
 */
class ta_init {
public:
    /**
     * Initialize the cut LP.
     */
    template <typename Problem, typename LP>
    void operator()(Problem & problem, LP & lp) {
        problem.init();
        lp.set_lp_name("Tree augmentation");
        lp.set_min_obj_fun();

        add_variables(problem, lp);

        add_cut_constraints(problem, lp);

        lp.load_matrix();
    }

private:
    /**
     * Adds a variable to the LP for each link in the input graph.
     * Binds the LP columns to graph edges.
     */
    template <typename Problem, typename LP>
    void add_variables(Problem & problem, LP & lp) {
        int eIdx{0};

        for (auto e : boost::make_iterator_range(edges(problem.get_links_graph()))) {
            std::string colName = get_edge_name(eIdx);
            lp::col_id colIdx = lp.add_column(problem.get_cost(e), lp::LO, 0, -1, colName);
            problem.bind_edge_to_col(e, colIdx);
            ++eIdx;
        }
    }

    /**
     * Adds a cut constraint to the LP for each edge in the input graph
     * and binds edges to LP rows.
     */
    template <typename Problem, typename LP>
    void add_cut_constraints(Problem & problem, LP & lp) {
        int dbIndex = 0;

        for (auto e : boost::make_iterator_range(edges(problem.get_tree_graph()))) {
            lp::row_id rowIdx = lp.add_row(lp::LO, 1, -1, get_row_name(dbIndex));
            problem.bind_edge_to_row(e, rowIdx);

            for (auto pe : problem.get_covered_by(e)) {
                lp.add_constraint_coef(rowIdx, problem.edge_to_col(pe));
            }
            ++dbIndex;
        }
    }

    std::string get_edge_name(int eIdx) const {
        return std::to_string(eIdx);
    }

    std::string get_cut_constr_prefix() const {
        return "cutConstraint";
    }

    std::string get_row_name(int dbIdx) const {
        return get_cut_constr_prefix() + std::to_string(dbIdx);
    }
};

template <
    typename Init = ta_init,
    typename RoundCondition = ta_round_condition,
    typename RelaxContition = ta_relax_condition>
        using tree_augmentation_ir_components = IRcomponents<Init, RoundCondition, RelaxContition>;

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
class tree_aug {
public:

    using Edge = typename boost::graph_traits<Graph>::edge_descriptor;
    using Vertex = typename boost::graph_traits<Graph>::vertex_descriptor;
    using CostValue = double;

    using TreeGraph = boost::filtered_graph<Graph, detail::bool_map_to_tree_filter<TreeMap>>;
    using NonTreeGraph = boost::filtered_graph<Graph, detail::bool_map_to_non_tree_filter<TreeMap>>;

    using EdgeList = std::vector<Edge>;
    using CoverMap = std::unordered_map<Edge, EdgeList, edge_hash<Graph>>;

    //cross reference between links and columns
    using EdgeToColId = boost::bimap<Edge, lp::col_id>;
    using RowIdToEdge = std::unordered_map<lp::row_id, Edge>;

    using ErrorMessage = boost::optional<std::string>;

    /**
     * Constructor.
     *
     * @param g  the graph to work with
     * @param treeMap designate a spanning tree in \c g
     * @param costMap costs of the links (=non-tree edges). The costs assigned to tree edges are not used.
     * @param solution result set of edges output iterator
     */
    tree_aug(const Graph & g, TreeMap treeMap, CostMap costMap, EdgeSetOutputIterator solution) :
        m_g(g), m_tree_map(treeMap), m_cost_map(costMap),
        m_solution(solution),
        m_tree(m_g, detail::bool_map_to_tree_filter<TreeMap>(m_tree_map)),
        m_ntree(m_g, detail::bool_map_to_non_tree_filter<TreeMap>(m_tree_map)),
        m_sol_cost(0)
    {}

    /// Checks validity of the input
    ErrorMessage check_input_validity() {
        // Num of edges == num of nodes-1 in the tree?
        int nV = num_vertices(m_g);
        int nE = filtered_num_edges(m_tree);

        if (nE != nV - 1) {
            return "Incorrect number of edges in the spanning tree. "
                        + std::string("Should be ") + std::to_string(nV - 1)
                        + ", but it is " + std::to_string(nE) + ".";
        }

        // Is the tree connected?
        std::vector<int> component(num_vertices(m_g));
        int num = boost::connected_components(m_tree, &component[0]);

        if (num > 1) {
            return  ErrorMessage{"The spanning tree is not connected."};
        }

        // Is the graph 2-edge-connected?
        detail::const_int_map<Edge, 1> const1EdgeMap;
        // TODO This stoer-wagner algorithm is unnecessarily slow for some reason
        int minCut = boost::stoer_wagner_min_cut(m_g, const1EdgeMap);
        if (minCut < 2) {
            return ErrorMessage{"The graph is not 2-edge-connected."};
        }

        return ErrorMessage{};
    }

    /**
     * Returns the non-tree graph (set of links).
     */
    const NonTreeGraph & get_links_graph() const {
        return m_ntree;
    }

    /**
     * Returns the spanning tree.
     */
    const TreeGraph & get_tree_graph() const {
        return m_tree;
    }

    /**
     * Returns the cost of an edge.
     */
    auto get_cost(Edge e) -> decltype(get(std::declval<CostMap>(), e)) {
        return get(m_cost_map, e);
    }

    /**
     * Adds an edge corresponding to the given LP column to the result set.
     */
    void add_to_solution(lp::col_id col) {
        *m_solution = m_edge_to_col_id.right.at(col);
        ++m_solution;
        m_sol_cost += m_cost_map[m_edge_to_col_id.right.at(col)];
        m_edge_to_col_id.right.erase(col);
    }

    /**
     * Binds a graph edge to a LP column.
     */
    void bind_edge_to_col(Edge e, lp::col_id col) {
        auto tmp = m_edge_to_col_id.insert(typename EdgeToColId::value_type(e, col));
        assert(tmp.second);
    }

    /**
     * Binds a graph edge to a LP row.
     */
    void bind_edge_to_row(Edge e, lp::row_id row) {
        auto tmp = m_row_id_to_edge.insert(typename RowIdToEdge::value_type(row, e));
        assert(tmp.second);
    }

    /**
     * Initializes the necessary data structures.
     */
    void init() {
        //We need to fill very useful auxiliary data structures:
        //\c m_covered_by - containing lists of
        //edges. For a tree edge \c t the list \c m_covered_by[t]
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
                        m_covered_by[pred[node]].push_back(e);
                        node = source(pred[node], m_tree);
                    }
                }
            }
        }
    }

    /**
     * Returns the edge corresponding to an LP row.
     */
    Edge row_to_edge(lp::row_id row) const {
        return m_row_id_to_edge.at(row);
    }

    /**
     * Returns the LP columnn corresponding to a graph edge.
     */
    lp::col_id edge_to_col(Edge e) const {
        return m_edge_to_col_id.left.at(e);
    }

    /**
     * Returns the list of links covering a given edge.
     */
    EdgeList & get_covered_by(Edge e) {
        return m_covered_by[e];
    }

    /**
     * Checks if an edge belongs to the solution.
     */
    bool is_in_solution(Edge e) const {
        return m_edge_to_col_id.left.find(e) == m_edge_to_col_id.left.end();
    }

    /**
     * Returns cost of the found solution.
     */
    CostValue get_solution_cost() const {
        return m_sol_cost;
    }

private:

    /// Input graph
    const Graph & m_g;
    /// Input tree edges map
    TreeMap m_tree_map;
    /// Input edge cost map
    CostMap m_cost_map;

    /// Which links are chosen in the solution
    EdgeSetOutputIterator m_solution;

    /// Auxiliary data structures
    EdgeToColId m_edge_to_col_id;

    /// The spanning tree
    TreeGraph  m_tree;
    /// The non-tree (=set of links)
    NonTreeGraph m_ntree;
    /// Cost of the solution found
    CostValue m_sol_cost;
    /// Structures for the "m_covered_by" relations
    CoverMap m_covered_by;
    /// Reference between tree edges and row ids
    RowIdToEdge m_row_id_to_edge;
};

namespace detail {
/**
 * @brief Creates a tree_aug object. Non-named parameters.
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
 * @return tree_aug object
 */
template <typename Graph, typename TreeMap, typename CostMap, typename EdgeSetOutputIterator>
tree_aug<Graph, TreeMap, CostMap, EdgeSetOutputIterator>
make_tree_aug(const Graph & g, TreeMap treeMap, CostMap costMap, EdgeSetOutputIterator solution) {
    return paal::ir::tree_aug<Graph, TreeMap, CostMap, EdgeSetOutputIterator>(g, treeMap, costMap, solution);
}

/**
 * @brief Solves the Tree Augmentation problem using Iterative Rounding. Non-named parameters.
 *
 * @tparam Graph
 * @tparam TreeMap
 * @tparam CostMap
 * @tparam EdgeSetOutputIterator
 * @tparam IRcomponents
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
          typename IRcomponents = tree_augmentation_ir_components<>,
          typename Visitor = trivial_visitor>
IRResult tree_augmentation_iterative_rounding(
        const Graph & g,
        TreeMap treeMap,
        CostMap costMap,
        EdgeSetOutputIterator solution,
        IRcomponents components = IRcomponents(),
        Visitor visitor = Visitor()) {
    auto treeaug = make_tree_aug(g, treeMap, costMap, solution);
    return solve_iterative_rounding(treeaug, std::move(components), std::move(visitor));
}
} // detail

/**
 * Creates a tree_aug object. Named parameters.
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
 * @return tree_aug object
 */
template <typename Graph, typename EdgeSetOutputIterator, typename P, typename T, typename R>
auto
make_tree_aug(const Graph & g, const boost::bgl_named_params<P, T, R> & params, EdgeSetOutputIterator solution) ->
        tree_aug<Graph,
                decltype(choose_const_pmap(get_param(params, boost::edge_color), g, boost::edge_color)),
                decltype(choose_const_pmap(get_param(params, boost::edge_weight), g, boost::edge_weight)),
                EdgeSetOutputIterator> {
    return detail::make_tree_aug(g,
            choose_const_pmap(get_param(params, boost::edge_color), g, boost::edge_color),
            choose_const_pmap(get_param(params, boost::edge_weight), g, boost::edge_weight),
            solution);
}

/**
 * Creates a tree_aug object. All default parameters.
 * The returned object can be used to check input validity or to get a lower bound on the
 * optimal solution cost.
 *
 * @tparam Graph
 * @tparam EdgeSetOutputIterator
 * @param g
 * @param solution
 *
 * @return tree_aug object
 */
template <typename Graph, typename EdgeSetOutputIterator>
auto
make_tree_aug(const Graph & g, EdgeSetOutputIterator solution) ->
        decltype(make_tree_aug(g, boost::no_named_parameters(), solution)) {
    return make_tree_aug(g, boost::no_named_parameters(), solution);
}

/**
 * @brief Solves the Tree Augmentation problem using Iterative Rounding. Named parameters.
 *
 * @tparam Graph
 * @tparam EdgeSetOutputIterator
 * @tparam IRcomponents
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
          typename IRcomponents = tree_augmentation_ir_components<>,
          typename Visitor = trivial_visitor,
          typename P, typename T, typename R>
IRResult tree_augmentation_iterative_rounding(
        const Graph & g,
        const boost::bgl_named_params<P, T, R> & params,
        EdgeSetOutputIterator solution,
        IRcomponents components = IRcomponents(),
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
 * @tparam IRcomponents
 * @tparam Visitor
 * @param g
 * @param solution
 * @param components
 * @param visitor
 *
 * @return solution status
 */
template <typename Graph, typename EdgeSetOutputIterator,
          typename IRcomponents = tree_augmentation_ir_components<>,
          typename Visitor = trivial_visitor>
IRResult tree_augmentation_iterative_rounding(
        const Graph & g,
        EdgeSetOutputIterator solution,
        IRcomponents components = IRcomponents(),
        Visitor visitor = Visitor()) {
    return tree_augmentation_iterative_rounding(g, boost::no_named_parameters(),
            std::move(solution), std::move(components), std::move(visitor));
}

} //ir
} //paal

#endif /* TREE_AUG_HPP */
