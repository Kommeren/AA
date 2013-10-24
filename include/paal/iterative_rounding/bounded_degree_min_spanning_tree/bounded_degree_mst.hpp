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
 * @brief this is a model of IRComponents concept.<br>
 * The LPSolve is ir::RowGenerationSolveLP. <br>
 * The RoundCondition is ir::RoundConditionEquals < 0 >.
 *
 * @tparam Graph input graph, has to be a model of boost::Graph
 * @tparam CostMap map from Graph edges to costs
 * @tparam DegreeBoundMap map from Graph vertices to degree bounds
 * @tparam OracleComponents components for separation oracle heuristics
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
              m_edgeMap(std::move(o.m_edgeMap)), m_edgeList(std::move(o.m_edgeList)),
              m_vertexMap(std::move(o.m_vertexMap)), m_vertexList(std::move(o.m_vertexList)),
              m_compare(std::move(o.m_compare))
    {}
                           
    typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;
    typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
    
    typedef boost::bimap<Edge, ColId> EdgeMap;
    typedef std::unordered_map<RowId, Vertex> VertexMap;
    typedef std::vector<Vertex> VertexList;
    
    typedef std::map<Edge, ColId> EdgeMapOriginal;
    typedef std::vector<Edge> EdgeList;

    typedef boost::optional<std::string> ErrorMessage;

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
    
    void removeColumn(ColId colId) {
        auto ret = m_edgeMap.right.erase(colId);
        assert(ret == 1);
    }
    
    void addColumnToSolution(ColId colId) {
        m_resultSpanningTree.insert(colToEdge(colId));
    }

    void bindEdgeToCol(Edge e, ColId col) {
        m_edgeMapOriginal.insert(typename EdgeMapOriginal::value_type(e, col));
        m_edgeMap.insert(typename EdgeMap::value_type(e, col));
    }

    int addEdge(Edge e) {
        m_edgeList.push_back(e);
        return m_edgeList.size() - 1;
    }

    int addVertex(Vertex v) {
        m_vertexList.push_back(v);
        return m_vertexList.size() - 1;
    }
    
    decltype(get(std::declval<CostMap>(), std::declval<Edge>()))
    getCost(Edge e) {
        return get(m_costMap, e);
    }
    
    decltype(get(std::declval<DegreeBoundMap>(), std::declval<Vertex>()))
    getDegreeBound(Vertex v) {
        return get(m_degBoundMap, v);
    }
    
    ColId edgeToCol(Edge e) {
        auto i = m_edgeMap.left.find(e);
        assert(i != m_edgeMap.left.end());
        return i->second;
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
        
    void addToResultSpanningTree(Edge e) {
        m_resultSpanningTree.insert(e);
    }
    
    utils::Compare<double> getCompare() const { 
        return m_compare;
    }

    void bindVertexToRow(Vertex v, RowId row) {
        m_vertexMap.insert(typename VertexMap::value_type(row, v));
    }

    void removeRow(RowId rowId) {
        auto ret = m_vertexMap.erase(rowId);
        assert(ret == 1);
    }

    boost::optional<Vertex> rowToVertex(RowId row) {
        auto i = m_vertexMap.find(row);
        if (i != m_vertexMap.end()) {
            return boost::optional<Vertex>(i->second);
        }
        else {
            return boost::optional<Vertex>();
        }
    }

private: 
    Edge colToEdge(ColId col) {
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
    EdgeList        m_edgeList;
    VertexMap       m_vertexMap;
    VertexList      m_vertexList;
    
    const utils::Compare<double>   m_compare;
};



/**
 * @brief make template function for BoundedDegreeMST, just to avoid providing type names in template.
 *
 * @tparam Graph
 * @tparam CostMap
 * @tparam DegreeBoundMap
 * @param g
 * @param costMap
 * @param degBoundMap
 *
 * @return 
 */
template <typename Graph, typename CostMap, typename DegreeBoundMap, typename ResultSpanningTree>
BoundedDegreeMST<Graph, CostMap, DegreeBoundMap, ResultSpanningTree>
make_BoundedDegreeMST(const Graph & g, const CostMap & costMap,
                      const DegreeBoundMap & degBoundMap, ResultSpanningTree & resultSpanningTree) {
    return BoundedDegreeMST<Graph, CostMap, DegreeBoundMap, ResultSpanningTree>(g, costMap, degBoundMap, resultSpanningTree);
}

template <typename Graph, typename CostMap, typename DegreeBoundMap,
          typename ResultSpanningTree, typename IRComponents,
          typename Visitor = TrivialVisitor>
void bounded_degree_mst_iterative_rounding(
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
     * @brief checks if the column of the LP can be rounded
     * @param lp LP object
     * @param col column number
     * @return true iff column can be rounded
     *
     * @tparam LP
     */
struct BDMSTRoundCondition {
    BDMSTRoundCondition(double epsilon = BoundedDegreeMSTCompareTraits::EPSILON) : m_roundZero(epsilon) {}
    
    template <typename Problem, typename LP>
    boost::optional<double> operator()(Problem & problem, const LP & lp, ColId col) {
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
     * @brief checks if the row of the LP can be relaxed
     * @param lp LP object
     * @param row row number
     * @return true iff row can be relaxed
     *
     * @tparam LP
     */
struct BDMSTRelaxCondition {
    template <typename Problem, typename LP>
    bool operator()(Problem & problem, const LP & lp, RowId row) {
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
     * @brief initializes the LP (variables for edges, degree bound constraints and constraint for all edges) 
     * @param lp LP object
     *
     * @tparam LP
     */
struct BDMSTInit {
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
     * @brief adds a variable to the LP for each edge in the input graph
     * @param lp LP object
     *
     * @tparam LP
     */
    template <typename Problem, typename LP>
    void addVariables(Problem & problem, LP & lp) {
        for(auto e : boost::make_iterator_range(edges(problem.getGraph()))) {
            auto eIdx = problem.addEdge(e);
            std::string colName = getEdgeName(eIdx);
            ColId col = lp.addColumn(problem.getCost(e), DB, 0, 1, colName);
            problem.bindEdgeToCol(e, col);
        }
    }
    
    /**
     * @brief adds a degree bound constraint to the LP for each vertex in the input graph
     * @param lp LP object
     *
     * @tparam LP
     */
    template <typename Problem, typename LP>
    void addDegreeBoundConstraints(Problem & problem, LP & lp) {
        auto const & g = problem.getGraph();
        
        for(auto v : boost::make_iterator_range(vertices(g))) {
            auto vIdx = problem.addVertex(v);
            RowId rowIdx = lp.addRow(UP, 0, problem.getDegreeBound(v), getDegreeBoundName(vIdx));
            problem.bindVertexToRow(v, rowIdx);
            auto adjEdges = out_edges(v, g);
            
            for(auto e : boost::make_iterator_range(adjEdges)) {
                lp.addConstraintCoef(rowIdx, problem.edgeToCol(e));
            }
        }
    }
    
    /**
     * @brief adds an equality constraint to the LP for the set of all edges in the input graph
     * @param lp LP object
     *
     * @tparam LP
     */
    template <typename Problem, typename LP>
    void addAllSetEquality(Problem & problem, LP & lp) {
        auto const & g = problem.getGraph();
        int vCnt = num_vertices(g);
        RowId rowIdx = lp.addRow(FX, vCnt-1, vCnt-1);
        
        for (ColId colIdx : boost::make_iterator_range(lp.getColumns())) {
            lp.addConstraintCoef(rowIdx, colIdx);
        }
    }
};
 

struct BDMSTSetSolution {
    BDMSTSetSolution(double epsilon = BoundedDegreeMSTCompareTraits::EPSILON) 
        : m_compare(epsilon) {}

    /**
     * @brief returns the generated spanning tree
     * @param solution Solution object
     * @param problem Problem object
     * @return generated spanning tree: map from input Graph edges to bool values (if the edge belongs to the tree)
     *
     * @tparam Solution
     */
    template <typename Problem, typename GetSolution>
    void operator()(Problem & problem, const GetSolution & solution) {
        for (auto edgeAndCol : problem.getOriginalEdgesMap()) {
            if(m_compare.e(solution(edgeAndCol.second), 1)) {
                problem.addToResultSpanningTree(edgeAndCol.first);
            }
        }
    }

private:
    const utils::Compare<double>   m_compare;
};

template <
         typename Graph,
         typename SolveLPToExtremePoint = RowGenerationSolveLP < BoundedDegreeMSTOracle < Graph, BoundedDegreeMSTOracleComponents<>> >, 
         typename RoundCondition = BDMSTRoundCondition, 
         typename RelaxContition = BDMSTRelaxCondition, 
         typename Init = BDMSTInit,
         typename SetSolution = BDMSTSetSolution>
             using  BDMSTIRComponents = IRComponents<SolveLPToExtremePoint, RoundCondition, RelaxContition, Init, SetSolution>;

} //ir
} //paal
#endif /* BOUNDED_DEGREE_MST_HPP */
