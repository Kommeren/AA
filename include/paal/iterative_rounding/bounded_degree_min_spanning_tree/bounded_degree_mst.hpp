/**
 * @file bounded_degree_mst.hpp
 * @brief 
 * @author Piotr Godlewski
 * @version 1.0
 * @date 2013-06-03
 */
#ifndef BOUNDED_DEGREE_MST_HPP
#define BOUNDED_DEGREE_MST_HPP

#include "paal/iterative_rounding/iterative_rounding.hpp"
#include "paal/iterative_rounding/ir_components.hpp"
#include "paal/iterative_rounding/lp_row_generation.hpp"
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
    BoundedDegreeMST(const Graph & g, const CostMap & costMap, const DegreeBoundMap & degBoundMap, ResultSpanningTree & resultSpanningTree) :
              m_g(g), m_costMap(costMap), m_degBoundMap(degBoundMap), m_resultSpanningTree(resultSpanningTree),
              m_compare(BoundedDegreeMSTCompareTraits::EPSILON)
    {}
              
    BoundedDegreeMST(BoundedDegreeMST && o) :
              m_g(o.m_g), m_costMap(o.m_costMap), m_degBoundMap(o.m_degBoundMap),
              m_resultSpanningTree(o.m_resultSpanningTree),
              m_edgeMap(std::move(o.m_edgeMap)), m_edgeList(std::move(o.m_edgeList)), m_vertexList(std::move(o.m_vertexList)),
              m_compare(std::move(o.m_compare))
    {}
                           
    typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;
    typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
    
    typedef boost::bimap<Edge, ColId> EdgeMap;
    typedef std::vector<Vertex> VertexList;
    
    typedef std::map<Edge, ColId> EdgeMapOriginal;
    typedef std::vector<Edge> EdgeList;
    
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
    
    decltype(boost::get(std::declval<CostMap>(), std::declval<Edge>()))
    getCost(Edge e) {
        return boost::get(m_costMap, e);
    }
    
    decltype(boost::get(std::declval<DegreeBoundMap>(), std::declval<Vertex>()))
    getDegree(Vertex v) {
        return boost::get(m_degBoundMap, v);
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
make_BoundedDegreeMST(const Graph & g, const CostMap & costMap, const DegreeBoundMap & degBoundMap, ResultSpanningTree & resultSpanningTree) {
    return BoundedDegreeMST<Graph, CostMap, DegreeBoundMap, ResultSpanningTree>(g, costMap, degBoundMap, resultSpanningTree);
}

template <typename Graph, typename CostMap, typename DegreeBoundMap, typename ResultSpanningTree, typename IRComponents, typename Visitor = TrivialVisitor>
void bounded_degree_mst_iterative_rounding(
        const Graph & g, 
        const CostMap & costMap, 
        const DegreeBoundMap & degBoundMap, 
        ResultSpanningTree & resultSpanningTree, 
        IRComponents comps,
        Visitor vis = Visitor()) {

        auto bdmst = make_BoundedDegreeMST(g, costMap, degBoundMap, resultSpanningTree);
        return solve_iterative_rounding(bdmst, std::move(comps), std::move(vis));
}

    /**
     * @brief checks if the row of the LP can be rounded
     * @param lp LP object
     * @param col row number
     * @return true iff row can be rounded
     *
     * @tparam LP
     */
struct BDMSTRoundCondition {
    BDMSTRoundCondition(double epsilon = BoundedDegreeMSTCompareTraits::EPSILON) : m_roundZero(epsilon) {}
    template <typename Solution, typename LP>
    boost::optional<double> operator()(Solution & sol, const LP & lp, ColId col) {
        auto ret = m_roundZero(sol, lp, col);
        if(ret) {
            sol.removeColumn(col);
        }
        return ret;
    }
private:
    RoundConditionEquals<0> m_roundZero;
};

/**
     * @brief initializes the LP (variables for edges, degree bound constraints and constraint for all edges) 
     * @param lp LP object
     *
     * @tparam LP
     */
struct BDMSTInit {
    template <typename Solution, typename LP>
    void operator()(Solution &sol, LP & lp) {
        lp.setLPName("bounded degree minimum spanning tree");
        lp.setMinObjFun(); 
        
        addVariables(sol, lp);
        addDegreeBoundConstraints(sol, lp);
        addAllSetEquality(sol, lp);
        
        lp.loadMatrix();
    }
private:
    std::string getDegBoundPrefix() const {
        return "degBound ";
    }
  
    std::string getDegBoundDesc(int dbIdx) const {
        return getDegBoundPrefix() + std::to_string(dbIdx);
    }
    
    std::string getEdgeName(int eIdx) const {
        return std::to_string(eIdx);
    }
    
    /**
     * @brief adds a variable to the LP for each edge in the input graph
     * @param lp LP object
     *
     * @tparam LP
     */
    template <typename Solution, typename LP>
    void addVariables(Solution & sol, LP & lp) {
        auto edges = boost::edges(sol.getGraph());
        
        for(auto e : utils::make_range(edges)) {
            auto eIdx = sol.addEdge(e);
            std::string colName = getEdgeName(eIdx);
            ColId col =  lp.addColumn( sol.getCost(e), DB, 0, 1, colName);
            sol.bindEdgeToCol(e, col);
        }
    }
    
    /**
     * @brief adds a degree bound constraint to the LP for each vertex in the input graph
     * @param lp LP object
     *
     * @tparam LP
     */
    template <typename Solution, typename LP>
    void addDegreeBoundConstraints(Solution & sol, LP & lp) {
        auto const & g = sol.getGraph();
        auto vertices = boost::vertices(g);
        
        for(auto v : utils::make_range(vertices)) {
            auto dbIdx = sol.addVertex(v);
            RowId rowIdx = lp.addRow(UP, 0, sol.getDegree(v) , getDegBoundDesc(dbIdx));
            auto adjEdges = boost::out_edges(v, g);
            
            for(auto e : utils::make_range(adjEdges)) {
                lp.addConstraintCoef(rowIdx, sol.edgeToCol(e));
            }
        }
    }
    
    /**
     * @brief adds an equality constraint to the LP for the set of all edges in the input graph
     * @param lp LP object
     *
     * @tparam LP
     */
    template <typename Solution, typename LP>
    void addAllSetEquality(Solution & sol, LP & lp) {
        auto const & g = sol.getGraph();
        int vCnt = boost::num_vertices(g);
        RowId rowIdx = lp.addRow(FX, vCnt-1, vCnt-1);
        
        for (ColId colIdx : utils::make_range(lp.getColumns())) {
            lp.addConstraintCoef(rowIdx, colIdx);
        }
    }
};
 

struct BDMSTSetSolution {
    BDMSTSetSolution(double epsilon = BoundedDegreeMSTCompareTraits::EPSILON) 
        : m_compare(epsilon) {}
    /**
     * @brief returns the generated spanning tree
     * @param getsol GetSolution object
     * @param sol Solution object
     * @return generated spanning tree: map from input Graph edges to bool values (if the edge belongs to the tree)
     *
     * @tparam GetSolution
     */
    template <typename Solution, typename GetSolution>
    void operator()(Solution & sol, const GetSolution & getsol) {
        for (auto edgeAndCol : sol.getOriginalEdgesMap()) {
            if(m_compare.e(getsol(edgeAndCol.second), 1)) {
                sol.addToResultSpanningTree(edgeAndCol.first);
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
         typename RelaxContition = utils::ReturnFalseFunctor, 
         typename Init = BDMSTInit,
         typename SetSolution = BDMSTSetSolution>
             using  BDMSTIRComponents = IRComponents<SolveLPToExtremePoint, RoundCondition, RelaxContition, Init, SetSolution>;

} //ir
} //paal
#endif /* BOUNDED_DEGREE_MST_HPP */
