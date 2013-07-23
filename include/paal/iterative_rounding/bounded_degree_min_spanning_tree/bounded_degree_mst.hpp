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
#include "paal/iterative_rounding/bounded_degree_min_spanning_tree/bounded_degree_mst_oracle_components.hpp"


namespace paal {
namespace ir {

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
template <typename Graph, typename CostMap, typename DegreeBoundMap,
          typename OracleComponents = BoundedDegreeMSTOracleComponents<> >
class BoundedDegreeMST : public IRComponents < RowGenerationSolveLP < BoundedDegreeMSTOracle < Graph, OracleComponents > >,
                                               RoundConditionEquals<0> > {
public:
    typedef BoundedDegreeMSTOracle< Graph, OracleComponents > Oracle;
    typedef RowGenerationSolveLP < Oracle > SolveLP;
    typedef RoundConditionEquals<0> RoundCondition;
    typedef IRComponents < SolveLP, RoundCondition > BoundedDegreeMSTBase;
  
    BoundedDegreeMST(const Graph & g, const CostMap & costMap, const DegreeBoundMap & degBoundMap) :
              BoundedDegreeMSTBase(SolveLP(m_separationOracle), RoundCondition(BoundedDegreeMST::EPSILON)),
              m_g(g), m_costMap(costMap), m_degBoundMap(degBoundMap),
              m_compare(BoundedDegreeMST::EPSILON),
              m_separationOracle(g, m_edgeMap, m_vertexList, m_compare)
    {}
              
    BoundedDegreeMST(BoundedDegreeMST && o) :
              BoundedDegreeMSTBase(SolveLP(m_separationOracle), RoundCondition(BoundedDegreeMST::EPSILON)),
              m_g(o.m_g), m_costMap(o.m_costMap), m_degBoundMap(o.m_degBoundMap),
              m_edgeMap(std::move(o.m_edgeMap)), m_edgeList(std::move(o.m_edgeList)), m_vertexList(std::move(o.m_vertexList)),
              m_spanningTree(std::move(o.m_spanningTree)),
              m_compare(std::move(o.m_compare)),
              m_separationOracle(m_g, m_edgeMap, m_vertexList, m_compare)
    {}
                           
    typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;
    typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
    
    typedef std::map<Edge, bool> SpanningTree;
    typedef typename Oracle::EdgeMap EdgeMap;
    typedef std::map<Edge, ColId> EdgeMapOriginal;
    typedef std::vector<Edge> EdgeList;
    typedef std::vector<Vertex> VertexList;
    
    /**
     * @brief checks if the row of the LP can be rounded
     * @param lp LP object
     * @param col row number
     * @return true iff row can be rounded
     *
     * @tparam LP
     */
    template <typename LP>
    boost::optional<double> roundCondition(const LP & lp, ColId col) {
        auto ret = BoundedDegreeMSTBase::roundCondition(lp, col);
        if(ret) {
            m_edgeMap.right.erase(col);
        }
        return ret;
        
    }

    /**
     * @brief initializes the LP (variables for edges, degree bound constraints and constraint for all edges) and the separation oracle
     * @param lp LP object
     *
     * @tparam LP
     */
    template <typename LP>
    void init(LP & lp) {
        lp.setLPName("bounded degree minimum spanning tree");
        lp.setMinObjFun(); 
        
        addVariables(lp);
        addDegreeBoundConstraints(lp);
        addAllSetEquality(lp);
        
        lp.loadMatrix();
    }
    
    /**
     * @brief returns the generated spanning tree
     * @param sol GetSolution object
     * @return generated spanning tree: map from input Graph edges to bool values (if the edge belongs to the tree)
     *
     * @tparam GetSolution
     */
    template <typename GetSolution>
    SpanningTree & getSolution(const GetSolution & sol) {
        if (!m_solutionGenerated) {
            generateSolution(sol);
        }
        return m_spanningTree;
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
    template <typename LP>
    void addVariables(LP & lp) {
        auto edges = boost::edges(m_g);
        int eIdx(0);
        m_edgeList.resize(boost::num_edges(m_g));
        
        for(Edge e : utils::make_range(edges)) {
            std::string colName = getEdgeName(eIdx);
            ColId col =  lp.addColumn(boost::get(m_costMap, e), DB, 0, 1, colName);
            m_edgeMap.insert(typename EdgeMap::value_type(e, col));
            m_edgeMapOriginal.insert(typename EdgeMapOriginal::value_type(e, col));
            m_spanningTree[e] = false;
            m_edgeList[eIdx] = e;
            ++eIdx;
        }
    }
    
    /**
     * @brief adds a degree bound constraint to the LP for each vertex in the input graph
     * @param lp LP object
     *
     * @tparam LP
     */
    template <typename LP>
    void addDegreeBoundConstraints(LP & lp) {
        int dbIdx(0);
        auto vertices = boost::vertices(m_g);
        m_vertexList.resize(boost::num_vertices(m_g));
        
        for(Vertex v : utils::make_range(vertices.first, vertices.second)) {
            RowId rowIdx = lp.addRow(UP, 0, boost::get(m_degBoundMap, v), getDegBoundDesc(dbIdx));
            auto adjEdges = boost::out_edges(v, m_g);
            
            for(Edge e : utils::make_range(adjEdges)) {
                lp.addConstraintCoef(rowIdx, m_edgeMap.left.at(e));
            }
            
            m_vertexList[dbIdx] = v;
            ++dbIdx;
        }
    }
    
    /**
     * @brief adds an equality constraint to the LP for the set of all edges in the input graph
     * @param lp LP object
     *
     * @tparam LP
     */
    template <typename LP>
    void addAllSetEquality(LP & lp) {
        int vCnt = boost::num_vertices(m_g);
        RowId rowIdx = lp.addRow(FX, vCnt-1, vCnt-1);
        
        for (ColId colIdx : utils::make_range(lp.getColumns())) {
            lp.addConstraintCoef(rowIdx, colIdx);
        }
    }
    
    bool isDegBoundName(const std::string & s) const {
        return s.compare(0, getDegBoundPrefix().size(), getDegBoundPrefix()) == 0;
    }
    
    int getDegBoundIndex(const std::string & s) const {
        return std::stoi( s.substr(getDegBoundPrefix().size(), s.size() - getDegBoundPrefix().size()) );
    }
    
    template <typename GetSolution>
    void generateSolution(const GetSolution & sol) {
        m_solutionGenerated = true;
        for (auto edgeAndCol : m_edgeMapOriginal) {
            if(m_compare.e(sol(edgeAndCol.second), 1)) {
                m_spanningTree[edgeAndCol.first] = true;
            }
        }
    }


    const Graph & m_g;
    const CostMap & m_costMap;
    const DegreeBoundMap & m_degBoundMap;
    
    EdgeMapOriginal m_edgeMapOriginal;
    EdgeMap         m_edgeMap;
    EdgeList        m_edgeList;
    VertexList      m_vertexList;
    
    SpanningTree    m_spanningTree;
    
    const utils::Compare<double>   m_compare;
    static const double EPSILON;
    bool m_solutionGenerated = false;
    
    Oracle m_separationOracle;
};

template <typename Graph, typename CostMap, typename DegreeBoundMap, typename OracleComponents>
const double BoundedDegreeMST<Graph, CostMap, DegreeBoundMap, OracleComponents>::EPSILON = 1e-10;


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
template <typename Graph, typename CostMap, typename DegreeBoundMap>
BoundedDegreeMST<Graph, CostMap, DegreeBoundMap>
make_BoundedDegreeMST(const Graph & g, const CostMap & costMap, const DegreeBoundMap & degBoundMap) {
    return BoundedDegreeMST<Graph, CostMap, DegreeBoundMap>(g, costMap, degBoundMap);
}

} //ir
} //paal
#endif /* BOUNDED_DEGREE_MST_HPP */
