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
 * @tparam CostMap map from Graph edges to costs
 * @tparam DegreeBoundMap map from Graph vertices to degree bounds
 * @tparam ResultSpanningTree
 */
template <typename Graph, typename CostMap, typename DegreeBoundMap, typename ResultSpanningTree>
class BoundedDegreeMST {
public:
    BoundedDegreeMST(const Graph & g, const CostMap & costMap, const DegreeBoundMap & degBoundMap,
                     ResultSpanningTree & resultSpanningTree) :
              m_g(g), m_costMap(costMap), m_degBoundMap(degBoundMap),
              m_resultSpanningTree(resultSpanningTree),
              m_compare(BoundedDegreeMSTCompareTraits::EPSILON)
    {}

    BoundedDegreeMST(BoundedDegreeMST && o) :
              m_g(o.m_g), m_costMap(o.m_costMap), m_degBoundMap(o.m_degBoundMap),
              m_resultSpanningTree(o.m_resultSpanningTree),
              m_edgeMap(std::move(o.m_edgeMap)),
              m_vertexMap(std::move(o.m_vertexMap)), m_vertexList(std::move(o.m_vertexList)),
              m_compare(std::move(o.m_compare))
    {}

    typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;
    typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;

    typedef boost::bimap<Edge, lp::ColId> EdgeMap;
    typedef std::unordered_map<lp::RowId, Vertex> VertexMap;
    typedef std::vector<Vertex> VertexList;

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

    const Graph & getGraph() {
        return m_g;
    }

    double getEpsilon() const {
        return m_compare.getEpsilon();
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
     * Adds a vertex to the vertex list.
     */
    int addVertex(Vertex v) {
        m_vertexList.push_back(v);
        return m_vertexList.size() - 1;
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
    decltype(get(std::declval<DegreeBoundMap>(), std::declval<Vertex>()))
    getDegreeBound(Vertex v) {
        return get(m_degBoundMap, v);
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

    const EdgeMap & getEdgeMap() const {
        return m_edgeMap;
    }

    const EdgeMapOriginal & getOriginalEdgesMap() const {
        return m_edgeMapOriginal;
    }

    const VertexList & getVertices() const {
        return m_vertexList;
    }

    /**
     * Adds an edge to the result spanning tree.
     */
    void addToResultSpanningTree(Edge e) {
        m_resultSpanningTree.insert(e);
    }

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
    const CostMap & m_costMap;
    const DegreeBoundMap & m_degBoundMap;
    ResultSpanningTree &  m_resultSpanningTree;

    EdgeMapOriginal m_edgeMapOriginal;
    EdgeMap         m_edgeMap;
    VertexMap       m_vertexMap;
    VertexList      m_vertexList;

    const utils::Compare<double>   m_compare;
};



/**
 * @brief Creates a BoundedDegreeMST object.
 *
 * @tparam Graph
 * @tparam CostMap
 * @tparam DegreeBoundMap
 * @tparam ResultSpanningTree
 * @param g
 * @param costMap
 * @param degBoundMap
 * @param resultSpanningTree
 *
 * @return BoundedDegreeMST object
 */
template <typename Graph, typename CostMap, typename DegreeBoundMap, typename ResultSpanningTree>
BoundedDegreeMST<Graph, CostMap, DegreeBoundMap, ResultSpanningTree>
make_BoundedDegreeMST(const Graph & g, const CostMap & costMap,
                      const DegreeBoundMap & degBoundMap, ResultSpanningTree & resultSpanningTree) {
    return BoundedDegreeMST<Graph, CostMap, DegreeBoundMap, ResultSpanningTree>(g, costMap, degBoundMap, resultSpanningTree);
}

/**
 * @brief Solves the Bounded Degree MST problem using Iterative Rounding.
 *
 * @tparam Graph
 * @tparam CostMap
 * @tparam DegreeBoundMap
 * @tparam ResultSpanningTree
 * @tparam IRComponents
 * @tparam Visitor
 * @param g
 * @param costMap
 * @param degBoundMap
 * @param resultSpanningTree
 * @param components
 * @param visitor
 *
 * @return solution status
 */
template <typename Graph, typename CostMap, typename DegreeBoundMap,
          typename ResultSpanningTree, typename IRComponents,
          typename Visitor = TrivialVisitor>
lp::ProblemType bounded_degree_mst_iterative_rounding(
        const Graph & g,
        const CostMap & costMap,
        const DegreeBoundMap & degBoundMap,
        ResultSpanningTree & resultSpanningTree,
        IRComponents components,
        Visitor visitor = Visitor()) {

        auto bdmst = make_BoundedDegreeMST(g, costMap, degBoundMap, resultSpanningTree);
        return solve_iterative_rounding(bdmst, std::move(components), std::move(visitor));
}

/**
 * Round Condition of the IR Bounded Degree MST algorithm.
 */
struct BDMSTRoundCondition {
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

        for(auto v : boost::make_iterator_range(vertices(g))) {
            auto vIdx = problem.addVertex(v);
            lp::RowId rowIdx = lp.addRow(lp::UP, 0, problem.getDegreeBound(v), getDegreeBoundName(vIdx));
            problem.bindVertexToRow(v, rowIdx);
            auto adjEdges = out_edges(v, g);

            for(auto e : boost::make_iterator_range(adjEdges)) {
                auto colId = problem.edgeToCol(e);
                assert(colId);
                lp.addConstraintCoef(rowIdx, *colId);
            }
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
         typename SolveLPToExtremePoint = lp::RowGenerationSolveLP < BoundedDegreeMSTOracle < Graph, BoundedDegreeMSTOracleComponents<>> >,
         typename RoundCondition = BDMSTRoundCondition,
         typename RelaxContition = BDMSTRelaxCondition,
         typename Init = BDMSTInit,
         typename SetSolution = BDMSTSetSolution>
             using  BDMSTIRComponents = IRComponents<SolveLPToExtremePoint, RoundCondition, RelaxContition, Init, SetSolution>;

} //ir
} //paal
#endif /* BOUNDED_DEGREE_MST_HPP */
