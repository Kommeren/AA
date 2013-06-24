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
    typedef IRComponents < RowGenerationSolveLP < BoundedDegreeMSTOracle < Graph, OracleComponents > >,
                           RoundConditionEquals<0> > BoundedDegreeMSTBase;
  
    BoundedDegreeMST(const Graph & g, const CostMap & costMap, const DegreeBoundMap & degBoundMap) :
              BoundedDegreeMSTBase(),
              m_g(g), m_costMap(costMap), m_degBoundMap(degBoundMap),
              m_solutionGenerated(false) { }
                           
    typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;
    typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
    
    typedef std::map<Edge, bool> SpanningTree;
    typedef std::map<Edge, std::string> EdgeNameMap;
    typedef std::vector<Edge> EdgeList;
    typedef std::vector<Vertex> VertexList;
    
    typedef utils::Compare<double> Compare;
   

    //CR jak przeniesiemy eppsilona (patrz na uwagi w  ir_components..), ot ta metoda nie jest potrzrebna
    //za to bedzie potrzeba zmienic cos w kontruktorze
    /**
     * @brief checks if column of the LP can be rounded to 0, with epsilon specific to the bounded degree MST problem
     * @param lp LP object
     * @param col column number
     * @return a pair consisting of a bool specifying if the column can be rounded and a double value to which the column is being rounded
     *
     * @tparam LP
     */
    template <typename LP>
    std::pair<bool, double> roundCondition(const LP & lp, int col) {
        return BoundedDegreeMSTBase::roundCondition(lp, col, BoundedDegreeMST::EPSILON);
    }
    
    /**
     * @brief checks if the row of the LP can be relaxed
     * @param lp LP object
     * @param col row number
     * @return true iff row can be relaxed
     *
     * @tparam LP
     */
    template <typename LP>
    bool relaxCondition(const LP & lp, int row) {
        if (isDegBoundName(lp.getRowName(row))) {
            int vIdx = getDegBoundIndex(lp.getRowName(row));
            Vertex v = m_vertexList[vIdx];
            //CR Sa juz zaimplementowane funkcje getRowDegree / getColDegree
            //Trzeba uwazac na te funckje przy zmianie semantyki usuwania kolumn/ wierszy bo one moga sie zepsuc...
            return (lp.getRowDegree(row) <= boost::get(m_degBoundMap, v) + 1);
        }
        else {
            return false;
        }
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
        
        BoundedDegreeMSTBase::m_solveLP.setOracle(&m_separationOracle);
        m_separationOracle.init(&m_g, &m_vertexList, &m_edgeMap, EPSILON);
    }
    
    /**
     * @brief returns the generated spanning tree
     * @param lp LP object
     * @return generated spanning tree: map from input Graph edges to bool values (if the edge belongs to the tree)
     *
     * @tparam LP
     */
    template <typename LP>
    SpanningTree & getSolution(const LP & lp) {
        if (!m_solutionGenerated) {
            generateSolution(lp);
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
        
        for(Edge e : utils::make_range(edges.first, edges.second)) {
            std::string colName = getEdgeName(eIdx);
            lp.addColumn(boost::get(m_costMap, e), DB, 0, 1, colName);
            m_edgeMap[e] = colName;
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
            int rowIdx = lp.addRow(UP, 0, boost::get(m_degBoundMap, v), getDegBoundDesc(dbIdx));
            auto adjVertices = boost::adjacent_vertices(v, m_g);
            
            for(const Vertex & u : utils::make_range(adjVertices.first, adjVertices.second)) {
                bool b; Edge e;
                std::tie(e, b) = boost::edge(v, u, m_g);
                
                if (b) {
                    lp.addConstraintCoef(rowIdx, lp.getColByName(m_edgeMap[e]));
                }
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
        int colCnt = lp.colSize();
        int rowIdx = lp.addRow(FX, vCnt-1, vCnt-1);
        
        for (int colIdx = 1; colIdx <= colCnt; ++colIdx) {
            lp.addConstraintCoef(rowIdx, colIdx);
        }
    }
    
    bool isDegBoundName(const std::string & s) const {
        return s.compare(0, getDegBoundPrefix().size(), getDegBoundPrefix()) == 0;
    }
    
    int getDegBoundIndex(const std::string & s) const {
        return std::stoi( s.substr(getDegBoundPrefix().size(), s.size() - getDegBoundPrefix().size()) );
    }
    
    template <typename LP>
    void generateSolution(const LP & lp) {
        int size = lp.colSize();
        
        for (int col = 1; col <= size; ++col) {
            m_spanningTree[ m_edgeList[ std::stoi(lp.getColName(col)) ] ] = true;
        }
        m_solutionGenerated = true;
    }


    const Graph & m_g;
    const CostMap & m_costMap;
    const DegreeBoundMap & m_degBoundMap;
    
    EdgeNameMap     m_edgeMap;
    EdgeList        m_edgeList;
    VertexList      m_vertexList;
    
    bool            m_solutionGenerated;
    SpanningTree    m_spanningTree;
    
    BoundedDegreeMSTOracle< Graph, OracleComponents > m_separationOracle;
    
    static const double EPSILON;
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
