/**
 * @file bounded_degree_mst.hpp
 * @brief
 * @author Piotr Godlewski
 * @version 1.0
 * @date 2013-06-03
 */
#ifndef BOUNDED_DEGREE_MST_HPP
#define BOUNDED_DEGREE_MST_HPP

#include <boost/graph/connected_components.hpp>

#include "paal/iterative_rounding/iterative_rounding.hpp"
#include "paal/iterative_rounding/ir_components.hpp"
#include "paal/lp/lp_row_generation.hpp"
#include "paal/iterative_rounding/bounded_degree_min_spanning_tree/bounded_degree_mst_oracle.hpp"


namespace paal {
namespace ir {



namespace {
struct BoundedDegreeMSTCompareTraits {
    static const double EPSILON;
};

const double BoundedDegreeMSTCompareTraits::EPSILON = 1e-10;
}


/**
 * @class BoundedDegreeMST
 * @brief The class for solving the Bounded Degree MST problem using Iterative Rounding.
 *
 * @tparam Graph input graph
 * @tparam DegreeBounds map from Graph vertices to degree bounds
 * @tparam CostMap map from Graph edges to costs
 * @tparam SpanningTreeOutputIterator
 */
template <typename Graph, typename DegreeBounds, typename CostMap, typename SpanningTreeOutputIterator>
class BoundedDegreeMST {
public:
    /**
     * Constructor.
     */
    BoundedDegreeMST(const Graph & g, const DegreeBounds & degBounds, CostMap costMap,
                     SpanningTreeOutputIterator resultSpanningTree) :
              m_g(g), m_costMap(costMap), m_degBounds(degBounds),
              m_resultSpanningTree(resultSpanningTree),
              m_compare(BoundedDegreeMSTCompareTraits::EPSILON)
    {}

    typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;
    typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;

    typedef boost::bimap<Edge, lp::ColId> EdgeMap;
    typedef std::unordered_map<lp::RowId, Vertex> VertexMap;

    typedef std::vector<std::pair<lp::ColId, Edge>> EdgeMapOriginal;

    typedef boost::optional<std::string> ErrorMessage;

    /**
     * Checks if the input graph is connected.
     */
    ErrorMessage checkInputValidity() {
        // Is g connected?
        std::vector<int> components(num_vertices(m_g));
        int num = boost::connected_components(m_g, &components[0]);

        if (num > 1) {
            return ErrorMessage("The graph is not connected.");
        }

        return ErrorMessage();
    }

    /**
     * Returns the input graph.
     */
    const Graph & getGraph() const {
        return m_g;
    }

    /**
     * Removes an LP column and the graph edge corresponding to it.
     */
    void removeColumn(lp::ColId colId) {
        auto ret = m_edgeMap.right.erase(colId);
        assert(ret == 1);
    }

    /**
     * Binds a graph edge to a LP column.
     */
    void bindEdgeToCol(Edge e, lp::ColId col) {
        m_edgeMapOriginal.push_back(typename EdgeMapOriginal::value_type(col, e));
        m_edgeMap.insert(typename EdgeMap::value_type(e, col));
    }

    /**
     * Returns the cost of a given edge.
     */
    decltype(get(std::declval<CostMap>(), std::declval<Edge>()))
    getCost(Edge e) {
        return get(m_costMap, e);
    }

    /**
     * Returns the degree bound of a vertex.
     */
    decltype(std::declval<DegreeBounds>()(std::declval<Vertex>()))
    getDegreeBound(Vertex v) {
        return m_degBounds(v);
    }

    /**
     * Returns the LP column corresponding to an edge, if it wasn't deleted from the LP.
     */
    boost::optional<lp::ColId> edgeToCol(Edge e) const {
        auto i = m_edgeMap.left.find(e);
        if (i != m_edgeMap.left.end()) {
            return boost::optional<lp::ColId>(i->second);
        }
        else {
            return boost::optional<lp::ColId>();
        }
    }

    /**
     * Returns a bimap between edges and LP column IDs.
     */
    const EdgeMap & getEdgeMap() const {
        return m_edgeMap;
    }

    /**
     * Returns a mapping between LP column IDs and edges in the original graph.
     */
    const EdgeMapOriginal & getOriginalEdgesMap() const {
        return m_edgeMapOriginal;
    }

    /**
     * Adds an edge to the result spanning tree.
     */
    void addToResultSpanningTree(Edge e) {
        *m_resultSpanningTree = e;
        ++m_resultSpanningTree;
    }

    /**
     * Returns the double comparison object.
     */
    utils::Compare<double> getCompare() const {
        return m_compare;
    }

    /**
     * Binds a graph vertex to an LP row.
     */
    void bindVertexToRow(Vertex v, lp::RowId row) {
        m_vertexMap.insert(typename VertexMap::value_type(row, v));
    }

    /**
     * Unbinds the graph vertex from its corresponding (deleted) LP row.
     */
    void removeRow(lp::RowId rowId) {
        auto ret = m_vertexMap.erase(rowId);
        assert(ret == 1);
    }

    /**
     * Returns the graph vertex corresponding to a given LP row,
     *        unless the row doen't correspond to any vertex.
     */
    boost::optional<Vertex> rowToVertex(lp::RowId row) {
        auto i = m_vertexMap.find(row);
        if (i != m_vertexMap.end()) {
            return boost::optional<Vertex>(i->second);
        }
        else {
            return boost::optional<Vertex>();
        }
    }

private:
    Edge colToEdge(lp::ColId col) {
        auto i = m_edgeMap.right.find(col);
        assert(i != m_edgeMap.right.end());
        return i->second;
    }

    const Graph & m_g;
    CostMap m_costMap;
    const DegreeBounds & m_degBounds;
    SpanningTreeOutputIterator m_resultSpanningTree;

    EdgeMapOriginal m_edgeMapOriginal;
    EdgeMap         m_edgeMap;
    VertexMap       m_vertexMap;

    const utils::Compare<double>   m_compare;
};



namespace detail {
/**
 * @brief Creates a BoundedDegreeMST object. Non-named version.
 *
 * @tparam Graph
 * @tparam DegreeBounds
 * @tparam CostMap
 * @tparam SpanningTreeOutputIterator
 * @param g
 * @param degBoundMap
 * @param costMap
 * @param resultSpanningTree
 *
 * @return BoundedDegreeMST object
 */
template <typename Graph, typename DegreeBounds, typename CostMap, typename SpanningTreeOutputIterator>
BoundedDegreeMST<Graph, DegreeBounds, CostMap, SpanningTreeOutputIterator>
make_BoundedDegreeMST(const Graph & g, const DegreeBounds & degBounds,
                      CostMap costMap, SpanningTreeOutputIterator resultSpanningTree) {
    return BoundedDegreeMST<Graph, DegreeBounds, CostMap, SpanningTreeOutputIterator>(g, degBounds, costMap, resultSpanningTree);
}

/**
 * @brief Solves the Bounded Degree MST problem using Iterative Rounding. Non-named version.
 *
 * @tparam Graph
 * @tparam DegreeBounds
 * @tparam CostMap
 * @tparam SpanningTreeOutputIterator
 * @tparam IRComponents
 * @tparam Visitor
 * @param g
 * @param degBoundMap
 * @param costMap
 * @param resultSpanningTree
 * @param components
 * @param visitor
 *
 * @return solution status
 */
template <typename Graph, typename DegreeBounds, typename CostMap,
          typename SpanningTreeOutputIterator, typename IRComponents,
          typename Visitor = TrivialVisitor>
IRResult bounded_degree_mst_iterative_rounding(
        const Graph & g,
        const DegreeBounds & degBounds,
        CostMap costMap,
        SpanningTreeOutputIterator resultSpanningTree,
        IRComponents components,
        Visitor visitor = Visitor()) {

    auto bdmst = make_BoundedDegreeMST(g, degBounds, costMap, resultSpanningTree);
    return solve_iterative_rounding(bdmst, std::move(components), std::move(visitor));
}
} // detail

/**
 * Creates a BoundedDegreeMST object. Named version.
 * The returned object can be used to check input validity or to get a lower bound on the
 * optimal solution cost.
 *
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
 *
 * @return BoundedDegreeMST object
 */
template <typename Graph, typename DegreeBounds, typename SpanningTreeOutputIterator,
            typename P, typename T, typename R>
auto
make_BoundedDegreeMST(const Graph & g,
                      const DegreeBounds & degBounds,
                      const boost::bgl_named_params<P, T, R> & params,
                      SpanningTreeOutputIterator resultSpanningTree)
        -> BoundedDegreeMST<Graph, DegreeBounds,
                decltype(choose_const_pmap(get_param(params, boost::edge_weight), g, boost::edge_weight)),
                SpanningTreeOutputIterator> {

    return detail::make_BoundedDegreeMST(g, degBounds,
                choose_const_pmap(get_param(params, boost::edge_weight), g, boost::edge_weight),
                resultSpanningTree);
}

/**
 * Creates a BoundedDegreeMST object. All default parameters.
 * The returned object can be used to check input validity or to get a lower bound on the
 * optimal solution cost.
 *
 * @tparam Graph
 * @tparam DegreeBounds
 * @tparam SpanningTreeOutputIterator
 * @param g
 * @param degBounds
 * @param resultSpanningTree
 *
 * @return BoundedDegreeMST object
 */
template <typename Graph, typename DegreeBounds, typename SpanningTreeOutputIterator>
auto
make_BoundedDegreeMST(const Graph & g, const DegreeBounds & degBounds,
                      SpanningTreeOutputIterator resultSpanningTree) ->
        decltype(make_BoundedDegreeMST(g, degBounds, boost::no_named_parameters(), resultSpanningTree)) {
    return make_BoundedDegreeMST(g, degBounds, boost::no_named_parameters(), resultSpanningTree);
}

/**
 * @brief Solves the Bounded Degree MST problem using Iterative Rounding. Named version.
 *
 * @tparam Graph
 * @tparam DegreeBounds
 * @tparam SpanningTreeOutputIterator
 * @tparam IRComponents
 * @tparam Visitor
 * @tparam P
 * @tparam T
 * @tparam R
 * @param g
 * @param degBounds
 * @param resultSpanningTree
 * @param params
 * @param components
 * @param visitor
 *
 * @return solution status
 */
template <typename Graph, typename DegreeBounds,
          typename SpanningTreeOutputIterator,
          typename IRComponents,
          typename Visitor = TrivialVisitor,
          typename P, typename T, typename R>
IRResult bounded_degree_mst_iterative_rounding(
            const Graph & g,
            const DegreeBounds & degBounds,
            const boost::bgl_named_params<P, T, R> & params,
            SpanningTreeOutputIterator resultSpanningTree,
            IRComponents components,
            Visitor visitor = Visitor()) {

        return detail::bounded_degree_mst_iterative_rounding(g, degBounds,
                    choose_const_pmap(get_param(params, boost::edge_weight), g, boost::edge_weight),
                    std::move(resultSpanningTree), std::move(components), std::move(visitor));
}

/**
 * @brief Solves the Bounded Degree MST problem using Iterative Rounding. All default parameters.
 *
 * @tparam Graph
 * @tparam DegreeBounds
 * @tparam SpanningTreeOutputIterator
 * @tparam IRComponents
 * @tparam Visitor
 * @param g
 * @param degBounds
 * @param resultSpanningTree
 * @param components
 * @param visitor
 *
 * @return solution status
 */
template <typename Graph, typename DegreeBounds,
          typename SpanningTreeOutputIterator,
          typename IRComponents,
          typename Visitor = TrivialVisitor>
IRResult bounded_degree_mst_iterative_rounding(
            const Graph & g,
            const DegreeBounds & degBounds,
            SpanningTreeOutputIterator resultSpanningTree,
            IRComponents components,
            Visitor visitor = Visitor()) {

        return bounded_degree_mst_iterative_rounding(g, degBounds,
                    boost::no_named_parameters(), std::move(resultSpanningTree),
                    std::move(components), std::move(visitor));
}

/**
 * Round Condition of the IR Bounded Degree MST algorithm.
 */
struct BDMSTRoundCondition {
    /**
     * Constructor. Takes epsilon used in double comparison.
     */
    BDMSTRoundCondition(double epsilon = BoundedDegreeMSTCompareTraits::EPSILON) : m_roundZero(epsilon) {}

    /**
     * Checks if a given column of the LP can be rounded to 0.
     * If the column is rounded, the corresponding edge is removed from the graph.
     */
    template <typename Problem, typename LP>
    boost::optional<double> operator()(Problem & problem, const LP & lp, lp::ColId col) {
        auto ret = m_roundZero(problem, lp, col);
        if (ret) {
            problem.removeColumn(col);
        }
        return ret;
    }

private:
    RoundConditionEquals<0> m_roundZero;
};

/**
 * Relax Condition of the IR Bounded Degree MST algorithm.
 */
struct BDMSTRelaxCondition {
    /**
     * Checks if a given row of the LP corresponds to a degree bound and can be relaxed.
     * If the row degree is not greater than the corresponding degree bound + 1, it is relaxed
     * and the degree bound is deleted from the problem.
     */
    template <typename Problem, typename LP>
    bool operator()(Problem & problem, const LP & lp, lp::RowId row) {
        auto vertex = problem.rowToVertex(row);
        if (vertex) {
            auto ret = (lp.getRowDegree(row) <= problem.getDegreeBound(*vertex) + 1);
            if (ret) {
                problem.removeRow(row);
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
struct BDMSTInit {
    /**
     * Initializes the LP: variables for edges, degree bound constraints
     * and constraint for all edges.
     */
    template <typename Problem, typename LP>
    void operator()(Problem & problem, LP & lp) {
        lp.setLPName("bounded degree minimum spanning tree");
        lp.setMinObjFun();

        addVariables(problem, lp);
        addDegreeBoundConstraints(problem, lp);
        addAllSetEquality(problem, lp);

        lp.loadMatrix();
    }

private:
    std::string getEdgeName(int eIdx) const {
        return std::to_string(eIdx);
    }

    std::string getDegreeBoundName(int vIdx) const {
        return "degBound" + std::to_string(vIdx);
    }

    /**
     * Adds a variable to the LP for each edge in the input graph.
     * Binds the LP columns to edges.
     */
    template <typename Problem, typename LP>
    void addVariables(Problem & problem, LP & lp) {
        for(auto e : boost::make_iterator_range(edges(problem.getGraph()))) {
            lp::ColId col = lp.addColumn(problem.getCost(e), lp::DB, 0, 1);
            problem.bindEdgeToCol(e, col);
        }
    }

    /**
     * Adds a degree bound constraint to the LP for each vertex in the input graph
     * and binds vertices to rows.
     */
    template <typename Problem, typename LP>
    void addDegreeBoundConstraints(Problem & problem, LP & lp) {
        auto const & g = problem.getGraph();
        int vIdx(0);

        for(auto v : boost::make_iterator_range(vertices(g))) {
            lp::RowId rowIdx = lp.addRow(lp::UP, 0, problem.getDegreeBound(v), getDegreeBoundName(vIdx));
            problem.bindVertexToRow(v, rowIdx);
            auto adjEdges = out_edges(v, g);

            for(auto e : boost::make_iterator_range(adjEdges)) {
                auto colId = problem.edgeToCol(e);
                assert(colId);
                lp.addConstraintCoef(rowIdx, *colId);
            }
            ++vIdx;
        }
    }

    /**
     * Adds an equality constraint to the LP for the set of all edges in the input graph.
     */
    template <typename Problem, typename LP>
    void addAllSetEquality(Problem & problem, LP & lp) {
        auto const & g = problem.getGraph();
        int vCnt = num_vertices(g);
        lp::RowId rowIdx = lp.addRow(lp::FX, vCnt-1, vCnt-1);

        for (lp::ColId colIdx : boost::make_iterator_range(lp.getColumns())) {
            lp.addConstraintCoef(rowIdx, colIdx);
        }
    }
};


/**
 * Set Solution component of the IR Bounded Degree MST algorithm.
 */
struct BDMSTSetSolution {
    /**
     * Constructor. Takes epsilon used in double comparison.
     */
    BDMSTSetSolution(double epsilon = BoundedDegreeMSTCompareTraits::EPSILON)
        : m_compare(epsilon) {}

    /**
     * Creates the result spanning tree form the LP (all edges corresponding to columns with value 1).
     */
    template <typename Problem, typename GetSolution>
    void operator()(Problem & problem, const GetSolution & solution) {
        for (auto edgeAndCol : problem.getOriginalEdgesMap()) {
            if (m_compare.e(solution(edgeAndCol.first), 1)) {
                problem.addToResultSpanningTree(edgeAndCol.second);
            }
        }
    }

private:
    const utils::Compare<double>   m_compare;
};

template <
         typename Graph,
         typename SolveLPToExtremePoint = lp::RowGenerationSolveLP<BoundedDegreeMSTOracle<>>,
         typename ResolveLPToExtremePoint = lp::RowGenerationResolveLP<BoundedDegreeMSTOracle<>>,
         typename RoundCondition = BDMSTRoundCondition,
         typename RelaxContition = BDMSTRelaxCondition,
         typename Init = BDMSTInit,
         typename SetSolution = BDMSTSetSolution>
             using  BDMSTIRComponents = IRComponents<SolveLPToExtremePoint, ResolveLPToExtremePoint,
                                RoundCondition, RelaxContition, Init, SetSolution>;

} //ir
} //paal
#endif /* BOUNDED_DEGREE_MST_HPP */
