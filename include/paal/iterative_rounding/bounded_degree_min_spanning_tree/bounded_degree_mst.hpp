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


namespace paal {
namespace ir {

template <typename Graph, typename CostMap, typename DegreeBoundMap>
class BoundedDegreeMST : public IRComponents<> {
public:
    BoundedDegreeMST(const Graph & g, const CostMap & costMap, const DegreeBoundMap & degBoundMap) :
              m_g(g), m_costMap(costMap), m_degBoundMap(degBoundMap) {}
                           
    typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;
    typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
    
    typedef std::map<Edge, bool> SpanningTree;
    typedef std::map<Edge, std::string> EdgeMap;
    typedef std::vector<Vertex> VertexList;
    
    typedef utils::Compare<double> Compare;
    
    template <typename LP>
    std::pair<bool, double> roundCondition(const LP & lp, int col) {
        auto ret = IRComponents<>::roundCondition(lp, col);
        ret.first = (ret.first && ret.second == 0);
        return ret;
    };
    
    template <typename LP>
    bool relaxCondition(const LP & lp, int row) {
        if (isDegBoundName(lp.getRowName(row))) {
            int vIdx = getDegBoundIndex(lp.getRowName(row));
            Vertex v = m_vertexList[vIdx];
            return Compare::le(nonZeroIncEdges(lp, v), m_degBoundMap[v] + 1);
        }
        else {
            return false;
        }
    };

    template <typename LP>
    void init(LP & lp) {
        lp.setLPName("bounded degree minimum spanning tree");
        lp.setMinObjFun(); 
        
        addVariables(lp);
        addDegreeBoundConstraints(lp);
        addAllSetEquality(lp);
        
        lp.loadMatrix();
    }
    
    SpanningTree & getSolution() {
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
        return "edge " + std::to_string(eIdx);
    }
    
    //adding variables
    template <typename LP>
    void addVariables(LP & lp) {
        auto edges = boost::edges(m_g);
        int eIdx(0);
        
        for(Edge e : utils::make_range(edges.first, edges.second)) {
            std::string colName = getEdgeName(eIdx);
            lp.addColumn(m_costMap[e], DB, 0, 1, colName);
            m_edgeMap[e] = colName;
            ++eIdx;
        }
    }
    
    //adding degree bound constraints
    template <typename LP>
    void addDegreeBoundConstraints(LP & lp) {
        int dbIdx(0);
        auto vertices = boost::vertices(m_g);
        m_vertexList.resize(num_vertices(m_g));
        
        for(Vertex v : utils::make_range(vertices.first, vertices.second)) {
            int rowIdx = lp.addRow(UP, 0, m_degBoundMap[v], getDegBoundDesc(dbIdx));
            auto adjVertices = adjacent_vertices(v, m_g);
            
            for(Vertex u : utils::make_range(adjVertices.first, adjVertices.second)) {
                bool b; Edge e;
                std::tie(e, b) = boost::edge(v, u, m_g);
                
                if (b) {
                    lp.addConstraintCoef(rowIdx, lp.getColByName(m_edgeMap[e]));
                }
                else {
                    // TODO fail
                }
            }
            
            m_vertexList[dbIdx] = v;
            ++dbIdx;
        }
    }
    
    //adding equality for the whole set
    template <typename LP>
    void addAllSetEquality(LP & lp) {
        int vCnt = num_vertices(m_g);
        int rowIdx = lp.addRow(FX, vCnt-1, vCnt-1);
        
        for (const std::pair<Edge, std::string> & e : m_edgeMap) {
            lp.addConstraintCoef(rowIdx, lp.getColByName(e.second));
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
        
        for(Vertex u : utils::make_range(adjVertices.first, adjVertices.second)) {
            bool b; Edge e;
            std::tie(e, b) = boost::edge(v, u, m_g);

            if (b) {
                int colIdx = lp.getColByName(m_edgeMap[e]);
                if (colIdx != 0 && !Compare::e(lp.getColUb(colIdx), 0)) {
                    ++nonZeroIncCnt;
                }
            }
            else {
              // TODO fail
            }
        }
        
        return nonZeroIncCnt;
    }

    const Graph & m_g;
    const CostMap & m_costMap;
    const DegreeBoundMap & m_degBoundMap;
    SpanningTree    m_spanningTree;
    EdgeMap         m_edgeMap;
    VertexList      m_vertexList;
};

template <typename Graph, typename CostMap, typename DegreeBoundMap>
BoundedDegreeMST<Graph, CostMap, DegreeBoundMap>
make_BoundedDegreeMST(const Graph & g, const CostMap & costMap, const DegreeBoundMap & degBoundMap) {
    return BoundedDegreeMST<Graph, CostMap, DegreeBoundMap>(g, costMap, degBoundMap);
}

} //ir
} //paal
#endif /* BOUNDED_DEGREE_MST_HPP */
