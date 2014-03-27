/**
 * @file bounded_degree_mst.hpp
 * @brief
 * @author Piotr Godlewski
 * @version 1.0
 * @date 2013-06-03
 */
#ifndef BOUNDED_DEGREE_MST_HPP
#define BOUNDED_DEGREE_MST_HPP

#include <boost/bimap.hpp>
#include <boost/graph/connected_components.hpp>

#include "paal/iterative_rounding/iterative_rounding.hpp"
#include "paal/iterative_rounding/ir_components.hpp"
#include "paal/lp/lp_row_generation.hpp"
#include "paal/iterative_rounding/bounded_degree_min_spanning_tree/bounded_degree_mst_oracle.hpp"
#include "paal/lp/separation_oracles.hpp"


namespace paal {
namespace ir {



namespace {
struct bounded_degree_mst_compare_traits {
    static const double EPSILON;
};

const double bounded_degree_mst_compare_traits::EPSILON = 1e-10;
}


template <template <typename> class OracleStrategy = lp::random_violated_separation_oracle>
using BDMSTOracle = OracleStrategy<bdmst_violation_checker>;


/**
 * @class bounded_degree_mst
 * @brief The class for solving the Bounded Degree MST problem using Iterative Rounding.
 *
 * @tparam Graph input graph
 * @tparam DegreeBounds map from Graph vertices to degree bounds
 * @tparam CostMap map from Graph edges to costs
 * @tparam SpanningTreeOutputIterator
 * @tparam Oracle separation oracle
 */
template <typename Graph, typename DegreeBounds, typename CostMap,
          typename SpanningTreeOutputIterator, typename Oracle = BDMSTOracle<>>
class bounded_degree_mst {
public:
    /**
     * Constructor.
     */
    bounded_degree_mst(const Graph & g, const DegreeBounds & degBounds, CostMap costMap,
                     SpanningTreeOutputIterator resultSpanningTree, Oracle oracle = Oracle()) :
              m_g(g), m_cost_map(costMap), m_deg_bounds(degBounds),
              m_result_spanning_tree(resultSpanningTree),
              m_compare(bounded_degree_mst_compare_traits::EPSILON),
              m_oracle(oracle)
    {}

    typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;
    typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;

    typedef boost::bimap<Edge, lp::col_id> EdgeMap;
    typedef std::unordered_map<lp::row_id, Vertex> VertexMap;

    typedef std::vector<std::pair<lp::col_id, Edge>> EdgeMapOriginal;

    typedef boost::optional<std::string> ErrorMessage;

    /**
     * Checks if the input graph is connected.
     */
    ErrorMessage check_input_validity() {
        // Is g connected?
        std::vector<int> components(num_vertices(m_g));
        int num = boost::connected_components(m_g, &components[0]);

        if (num > 1) {
            return ErrorMessage("The graph is not connected.");
        }

        return ErrorMessage();
    }

    /**
     * Returns the separation oracle.
     */
    Oracle & get_oracle() {
        return m_oracle;
    }

    /**
     * Returns the input graph.
     */
    const Graph & get_graph() const {
        return m_g;
    }

    /**
     * Removes an LP column and the graph edge corresponding to it.
     */
    void remove_column(lp::col_id colId) {
        auto ret = m_edge_map.right.erase(colId);
        assert(ret == 1);
    }

    /**
     * Binds a graph edge to a LP column.
     */
    void bind_edge_to_col(Edge e, lp::col_id col) {
        m_edge_map_original.push_back(typename EdgeMapOriginal::value_type(col, e));
        m_edge_map.insert(typename EdgeMap::value_type(e, col));
    }

    /**
     * Returns the cost of a given edge.
     */
    decltype(get(std::declval<CostMap>(), std::declval<Edge>()))
    get_cost(Edge e) {
        return get(m_cost_map, e);
    }

    /**
     * Returns the degree bound of a vertex.
     */
    decltype(std::declval<DegreeBounds>()(std::declval<Vertex>()))
    get_degree_bound(Vertex v) {
        return m_deg_bounds(v);
    }

    /**
     * Returns the LP column corresponding to an edge, if it wasn't deleted from the LP.
     */
    boost::optional<lp::col_id> edge_to_col(Edge e) const {
        auto i = m_edge_map.left.find(e);
        if (i != m_edge_map.left.end()) {
            return boost::optional<lp::col_id>(i->second);
        }
        else {
            return boost::optional<lp::col_id>();
        }
    }

    /**
     * Returns a bimap between edges and LP column IDs.
     */
    const EdgeMap & get_edge_map() const {
        return m_edge_map;
    }

    /**
     * Returns a mapping between LP column IDs and edges in the original graph.
     */
    const EdgeMapOriginal & get_original_edges_map() const {
        return m_edge_map_original;
    }

    /**
     * Adds an edge to the result spanning tree.
     */
    void add_to_result_spanning_tree(Edge e) {
        *m_result_spanning_tree = e;
        ++m_result_spanning_tree;
    }

    /**
     * Returns the double comparison object.
     */
    utils::Compare<double> get_compare() const {
        return m_compare;
    }

    /**
     * Binds a graph vertex to an LP row.
     */
    void bind_vertex_to_row(Vertex v, lp::row_id row) {
        m_vertex_map.insert(typename VertexMap::value_type(row, v));
    }

    /**
     * Unbinds the graph vertex from its corresponding (deleted) LP row.
     */
    void remove_row(lp::row_id rowId) {
        auto ret = m_vertex_map.erase(rowId);
        assert(ret == 1);
    }

    /**
     * Returns the graph vertex corresponding to a given LP row,
     *        unless the row doen't correspond to any vertex.
     */
    boost::optional<Vertex> row_to_vertex(lp::row_id row) {
        auto i = m_vertex_map.find(row);
        if (i != m_vertex_map.end()) {
            return boost::optional<Vertex>(i->second);
        }
        else {
            return boost::optional<Vertex>();
        }
    }

private:
    Edge col_to_edge(lp::col_id col) {
        auto i = m_edge_map.right.find(col);
        assert(i != m_edge_map.right.end());
        return i->second;
    }

    const Graph & m_g;
    CostMap m_cost_map;
    const DegreeBounds & m_deg_bounds;
    SpanningTreeOutputIterator m_result_spanning_tree;

    EdgeMapOriginal m_edge_map_original;
    EdgeMap         m_edge_map;
    VertexMap       m_vertex_map;

    const utils::Compare<double>   m_compare;

    Oracle m_oracle;
};



namespace detail {
/**
 * @brief Creates a bounded_degree_mst object. Non-named version.
 *
 * @tparam Oracle
 * @tparam Graph
 * @tparam DegreeBounds
 * @tparam CostMap
 * @tparam SpanningTreeOutputIterator
 * @param g
 * @param degBoundMap
 * @param costMap
 * @param resultSpanningTree
 * @param oracle
 *
 * @return bounded_degree_mst object
 */
template <typename Oracle = BDMSTOracle<>, typename Graph,
          typename DegreeBounds, typename CostMap, typename SpanningTreeOutputIterator>
bounded_degree_mst<Graph, DegreeBounds, CostMap, SpanningTreeOutputIterator, Oracle>
make_bounded_degree_mst(const Graph & g, const DegreeBounds & degBounds,
                      CostMap costMap, SpanningTreeOutputIterator resultSpanningTree,
                      Oracle oracle = Oracle()) {
    return bounded_degree_mst<Graph, DegreeBounds, CostMap,
                SpanningTreeOutputIterator, Oracle>(g, degBounds, costMap, resultSpanningTree, oracle);
}
} // detail

/**
 * Creates a bounded_degree_mst object. Named version.
 * The returned object can be used to check input validity or to get a lower bound on the
 * optimal solution cost.
 *
 * @tparam Oracle
 * @tparam Graph
 * @tparam DegreeBounds
 * @tparam SpanningTreeOutputIterator
 * @tparam P
 * @tparam T
 * @tparam R
 * @param g
 * @param degBounds
 * @param params
 * @param resultSpanningTree
 * @param oracle
 *
 * @return bounded_degree_mst object
 */
template <typename Oracle = BDMSTOracle<>,
          typename Graph, typename DegreeBounds, typename SpanningTreeOutputIterator,
          typename P, typename T, typename R>
auto
make_bounded_degree_mst(const Graph & g,
                      const DegreeBounds & degBounds,
                      const boost::bgl_named_params<P, T, R> & params,
                      SpanningTreeOutputIterator resultSpanningTree,
                      Oracle oracle = Oracle())
        -> bounded_degree_mst<Graph, DegreeBounds,
                decltype(choose_const_pmap(get_param(params, boost::edge_weight), g, boost::edge_weight)),
                SpanningTreeOutputIterator, Oracle> {

    return detail::make_bounded_degree_mst(g, degBounds,
                choose_const_pmap(get_param(params, boost::edge_weight), g, boost::edge_weight),
                resultSpanningTree, oracle);
}

/**
 * Creates a bounded_degree_mst object. All default parameters.
 * The returned object can be used to check input validity or to get a lower bound on the
 * optimal solution cost.
 *
 * @tparam Oracle
 * @tparam Graph
 * @tparam DegreeBounds
 * @tparam SpanningTreeOutputIterator
 * @param g
 * @param degBounds
 * @param resultSpanningTree
 * @param oracle
 *
 * @return bounded_degree_mst object
 */
template <typename Oracle = BDMSTOracle<>,
          typename Graph, typename DegreeBounds, typename SpanningTreeOutputIterator>
auto
make_bounded_degree_mst(const Graph & g, const DegreeBounds & degBounds,
                      SpanningTreeOutputIterator resultSpanningTree,
                      Oracle oracle = Oracle()) ->
        decltype(make_bounded_degree_mst(g, degBounds, boost::no_named_parameters(), resultSpanningTree, oracle)) {
    return make_bounded_degree_mst(g, degBounds, boost::no_named_parameters(), resultSpanningTree, oracle);
}



/**
 * Round Condition of the IR Bounded Degree MST algorithm.
 */
struct bdmst_round_condition {
    /**
     * Constructor. Takes epsilon used in double comparison.
     */
    bdmst_round_condition(double epsilon = bounded_degree_mst_compare_traits::EPSILON) : m_round_zero(epsilon) {}

    /**
     * Checks if a given column of the LP can be rounded to 0.
     * If the column is rounded, the corresponding edge is removed from the graph.
     */
    template <typename Problem, typename LP>
    boost::optional<double> operator()(Problem & problem, const LP & lp, lp::col_id col) {
        auto ret = m_round_zero(problem, lp, col);
        if (ret) {
            problem.remove_column(col);
        }
        return ret;
    }

private:
    round_condition_equals<0> m_round_zero;
};

/**
 * Relax Condition of the IR Bounded Degree MST algorithm.
 */
struct bdmst_relax_condition {
    /**
     * Checks if a given row of the LP corresponds to a degree bound and can be relaxed.
     * If the row degree is not greater than the corresponding degree bound + 1, it is relaxed
     * and the degree bound is deleted from the problem.
     */
    template <typename Problem, typename LP>
    bool operator()(Problem & problem, const LP & lp, lp::row_id row) {
        auto vertex = problem.row_to_vertex(row);
        if (vertex) {
            auto ret = (lp.get_row_degree(row) <= problem.get_degree_bound(*vertex) + 1);
            if (ret) {
                problem.remove_row(row);
            }
            return ret;
        }
        else
            return false;
    }
};

/**
 * Initialization of the IR Bounded Degree MST algorithm.
 */
struct bdmst_init {
    /**
     * Initializes the LP: variables for edges, degree bound constraints
     * and constraint for all edges.
     */
    template <typename Problem, typename LP>
    void operator()(Problem & problem, LP & lp) {
        lp.set_lp_name("bounded degree minimum spanning tree");
        lp.set_min_obj_fun();

        add_variables(problem, lp);
        add_degree_bound_constraints(problem, lp);
        add_all_set_equality(problem, lp);

        lp.load_matrix();
    }

private:
    std::string get_edge_name(int eIdx) const {
        return std::to_string(eIdx);
    }

    std::string get_degree_bound_name(int vIdx) const {
        return "degBound" + std::to_string(vIdx);
    }

    /**
     * Adds a variable to the LP for each edge in the input graph.
     * Binds the LP columns to edges.
     */
    template <typename Problem, typename LP>
    void add_variables(Problem & problem, LP & lp) {
        for(auto e : boost::make_iterator_range(edges(problem.get_graph()))) {
            lp::col_id col = lp.add_column(problem.get_cost(e), lp::DB, 0, 1);
            problem.bind_edge_to_col(e, col);
        }
    }

    /**
     * Adds a degree bound constraint to the LP for each vertex in the input graph
     * and binds vertices to rows.
     */
    template <typename Problem, typename LP>
    void add_degree_bound_constraints(Problem & problem, LP & lp) {
        auto const & g = problem.get_graph();
        int vIdx(0);

        for(auto v : boost::make_iterator_range(vertices(g))) {
            lp::row_id rowIdx = lp.add_row(lp::UP, 0, problem.get_degree_bound(v), get_degree_bound_name(vIdx));
            problem.bind_vertex_to_row(v, rowIdx);
            auto adjEdges = out_edges(v, g);

            for(auto e : boost::make_iterator_range(adjEdges)) {
                auto colId = problem.edge_to_col(e);
                assert(colId);
                lp.add_constraint_coef(rowIdx, *colId);
            }
            ++vIdx;
        }
    }

    /**
     * Adds an equality constraint to the LP for the set of all edges in the input graph.
     */
    template <typename Problem, typename LP>
    void add_all_set_equality(Problem & problem, LP & lp) {
        auto const & g = problem.get_graph();
        int vCnt = num_vertices(g);
        lp::row_id rowIdx = lp.add_row(lp::FX, vCnt-1, vCnt-1);

        for (lp::col_id colIdx : boost::make_iterator_range(lp.get_columns())) {
            lp.add_constraint_coef(rowIdx, colIdx);
        }
    }
};


/**
 * Set Solution component of the IR Bounded Degree MST algorithm.
 */
struct bdmst_set_solution {
    /**
     * Constructor. Takes epsilon used in double comparison.
     */
    bdmst_set_solution(double epsilon = bounded_degree_mst_compare_traits::EPSILON)
        : m_compare(epsilon) {}

    /**
     * Creates the result spanning tree form the LP (all edges corresponding to columns with value 1).
     */
    template <typename Problem, typename GetSolution>
    void operator()(Problem & problem, const GetSolution & solution) {
        for (auto edgeAndCol : problem.get_original_edges_map()) {
            if (m_compare.e(solution(edgeAndCol.first), 1)) {
                problem.add_to_result_spanning_tree(edgeAndCol.second);
            }
        }
    }

private:
    const utils::Compare<double>   m_compare;
};

template <
         typename SolveLPToExtremePoint = lp::row_generation_solve_lp,
         typename Resolve_lp_to_extreme_point = lp::row_generation_resolve_lp,
         typename RoundCondition = bdmst_round_condition,
         typename RelaxContition = bdmst_relax_condition,
         typename Init = bdmst_init,
         typename SetSolution = bdmst_set_solution>
             using  BDMSTIRcomponents = IRcomponents<SolveLPToExtremePoint, Resolve_lp_to_extreme_point,
                                RoundCondition, RelaxContition, Init, SetSolution>;


namespace detail {
/**
 * @brief Solves the Bounded Degree MST problem using Iterative Rounding. Non-named version.
 *
 * @tparam Oracle
 * @tparam Graph
 * @tparam DegreeBounds
 * @tparam CostMap
 * @tparam SpanningTreeOutputIterator
 * @tparam IRcomponents
 * @tparam Visitor
 * @param g
 * @param degBoundMap
 * @param costMap
 * @param resultSpanningTree
 * @param components
 * @param oracle
 * @param visitor
 *
 * @return solution status
 */
template <typename Oracle = BDMSTOracle<>,
          typename Graph, typename DegreeBounds, typename CostMap,
          typename SpanningTreeOutputIterator,
          typename IRcomponents = BDMSTIRcomponents<>,
          typename Visitor = trivial_visitor>
IRResult bounded_degree_mst_iterative_rounding(
        const Graph & g,
        const DegreeBounds & degBounds,
        CostMap costMap,
        SpanningTreeOutputIterator resultSpanningTree,
        IRcomponents components = IRcomponents(),
        Oracle oracle = Oracle(),
        Visitor visitor = Visitor()) {

    auto bdmst = make_bounded_degree_mst(g, degBounds, costMap, resultSpanningTree, oracle);
    return solve_iterative_rounding(bdmst, std::move(components), std::move(visitor));
}
} // detail


/**
 * @brief Solves the Bounded Degree MST problem using Iterative Rounding. Named version.
 *
 * @tparam Oracle
 * @tparam Graph
 * @tparam DegreeBounds
 * @tparam SpanningTreeOutputIterator
 * @tparam IRcomponents
 * @tparam Visitor
 * @tparam P
 * @tparam T
 * @tparam R
 * @param g
 * @param degBounds
 * @param resultSpanningTree
 * @param params
 * @param components
 * @param oracle
 * @param visitor
 *
 * @return solution status
 */
template <typename Oracle = BDMSTOracle<>,
          typename Graph, typename DegreeBounds,
          typename SpanningTreeOutputIterator,
          typename IRcomponents = BDMSTIRcomponents<>,
          typename Visitor = trivial_visitor,
          typename P, typename T, typename R>
IRResult bounded_degree_mst_iterative_rounding(
            const Graph & g,
            const DegreeBounds & degBounds,
            const boost::bgl_named_params<P, T, R> & params,
            SpanningTreeOutputIterator resultSpanningTree,
            IRcomponents components = IRcomponents(),
            Oracle oracle = Oracle(),
            Visitor visitor = Visitor()) {

        return detail::bounded_degree_mst_iterative_rounding(g, degBounds,
                    choose_const_pmap(get_param(params, boost::edge_weight), g, boost::edge_weight),
                    std::move(resultSpanningTree), std::move(components),
                    std::move(oracle), std::move(visitor));
}

/**
 * @brief Solves the Bounded Degree MST problem using Iterative Rounding. All default parameters.
 *
 * @tparam Oracle
 * @tparam Graph
 * @tparam DegreeBounds
 * @tparam SpanningTreeOutputIterator
 * @tparam IRcomponents
 * @tparam Visitor
 * @param g
 * @param degBounds
 * @param resultSpanningTree
 * @param components
 * @param oracle
 * @param visitor
 *
 * @return solution status
 */
template <typename Oracle = BDMSTOracle<>,
          typename Graph, typename DegreeBounds,
          typename SpanningTreeOutputIterator,
          typename IRcomponents = BDMSTIRcomponents<>,
          typename Visitor = trivial_visitor>
IRResult bounded_degree_mst_iterative_rounding(
            const Graph & g,
            const DegreeBounds & degBounds,
            SpanningTreeOutputIterator resultSpanningTree,
            IRcomponents components = IRcomponents(),
            Oracle oracle = Oracle(),
            Visitor visitor = Visitor()) {

        return bounded_degree_mst_iterative_rounding(g, degBounds,
                    boost::no_named_parameters(), std::move(resultSpanningTree),
                    std::move(components), std::move(oracle), std::move(visitor));
}


} //ir
} //paal
#endif /* BOUNDED_DEGREE_MST_HPP */
