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

#include "paal/iterative_rounding/iterative_rounding.hpp"
#include "paal/iterative_rounding/ir_components.hpp"
#include "paal/iterative_rounding/lp_row_generation.hpp"
#include "paal/iterative_rounding/steiner_network/steiner_network_oracle.hpp"


namespace paal {
namespace ir {

template <typename Graph, typename CostMap, typename Restrictions>
class SteinerNetwork : public IRComponents < RowGenerationSolveLP < SteinerNetworkOracle < Graph, Restrictions> >,
                                               RoundConditionEquals<0> > {
public:
    typedef SteinerNetworkOracle < Graph, Restrictions > Oracle;
    typedef IRComponents < RowGenerationSolveLP < Oracle >,
                           RoundConditionEquals<0> > base;
  
    SteinerNetwork(const Graph & g, const CostMap & costMap, const Restrictions & restrictions) :
            base(RowGenerationSolveLP<Oracle>(m_separationOracle)),
            m_g(g), m_costMap(costMap), m_restrictions(restrictions),
            m_solutionGenerated(false), m_separationOracle(m_g, m_restrictions, m_edgeMap, m_compare) { }
                           
    typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;
    typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
    
    typedef std::map<Edge, bool> SpanningTree;
    typedef std::map<Edge, std::string> EdgeNameMap;
    typedef std::vector<Edge> EdgeList;
    typedef std::vector<Vertex> VertexList;
    
    typedef utils::Compare<double> Compare;
    

    template <typename LP>
    void init(LP & lp) {
        lp.setLPName("steiner network tree");
        lp.setMinObjFun(); 
        
        addVariables(lp);
        lp.loadMatrix();
    }
    
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
    
    //adding variables
    template <typename LP>
    void addVariables(LP & lp) {
        auto edges = boost::edges(m_g);
        int eIdx(0);
        m_edgeList.resize(num_edges(m_g));
        
        for(Edge e : utils::make_range(edges.first, edges.second)) {
            std::string colName = getEdgeName(eIdx);
            lp.addColumn(m_costMap[e], DB, 0, 1, colName);
            m_edgeMap[e] = colName;
            m_spanningTree[e] = false;
            m_edgeList[eIdx] = e;
            ++eIdx;
        }
    }
    
    bool isDegBoundName(const std::string & s) const {
        return s.compare(0, getDegBoundPrefix().size(), getDegBoundPrefix()) == 0;
    }
    
    int getDegBoundIndex(const std::string & s) const {
        return std::stoi( s.substr(getDegBoundPrefix().size(), s.size() - getDegBoundPrefix().size()) );
    }
    
    template <typename LP>
    int nonZeroIncEdges(const LP & lp, const Vertex & v) {
        int nonZeroIncCnt(0);
        auto adjVertices = adjacent_vertices(v, m_g);
        
        for(const Vertex & u : utils::make_range(adjVertices.first, adjVertices.second)) {
            bool b; Edge e;
            std::tie(e, b) = boost::edge(v, u, m_g);

            if (b) {
                int colIdx = lp.getColByName(m_edgeMap[e]);
                if (colIdx != 0 && !m_compare.e(lp.getColUb(colIdx), 0)) {
                    ++nonZeroIncCnt;
                }
            }
        }
        
        return nonZeroIncCnt;
    }
    
    template <typename LP>
    void generateSolution(const LP & lp) {
        int size = lp.colSize();
        
        for (int col = 1; col <= size; ++col) {
            m_spanningTree[ m_edgeList[ std::stoi(lp.getColName(col)) ] ] = true;
        }
        m_solutionGenerated = true;
    }

private:
    const Graph & m_g;
    const CostMap & m_costMap;
    const Restrictions & m_restrictions;
    
    EdgeNameMap     m_edgeMap;
    EdgeList        m_edgeList;
    VertexList      m_vertexList;
    
    bool            m_solutionGenerated;
    SpanningTree    m_spanningTree;
    
    Compare m_compare;
    Oracle m_separationOracle;
};

template <typename Graph, typename CostMap, typename Restrictions>
SteinerNetwork<Graph, CostMap, Restrictions>
make_SteinerNetwork(const Graph & g, const CostMap & costMap, const Restrictions & rest) {
    return SteinerNetwork<Graph, CostMap, Restrictions>(g, costMap, rest);
}

} //ir
} //paal
#endif /* STEINER_NETWORK_HPP */
