/**
 * @file bounded_degree_mst_oracle.hpp
 * @brief 
 * @author Piotr Godlewski
 * @version 1.0
 * @date 2013-06-05
 */
#ifndef BOUNDED_DEGREE_MST_ORACLE_HPP
#define BOUNDED_DEGREE_MST_ORACLE_HPP

#include <boost/graph/adjacency_list.hpp>

#include "paal/utils/double_rounding.hpp"


namespace paal {
namespace ir {

template <typename Graph>
class BoundedDegreeMSTOracle {
public:
    typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;
    typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
    
    typedef std::map<Edge, std::string> EdgeNameMap;
//     typedef std::vector<Edge> EdgeList;
    typedef std::vector<Vertex> VertexList;
    
    BoundedDegreeMSTOracle() : m_g(0), m_edgeNameMap(0)
    { }
    
    void init(const Graph * g, const VertexList * vertexList, const EdgeNameMap * edgeMap) {
        m_g = g;
        m_vertexList = vertexList;
        m_edgeNameMap = edgeMap;
        
        m_cap = boost::get(boost::edge_capacity, m_auxGraph);
        m_rev = boost::get(boost::edge_reverse, m_auxGraph);
        m_resCap = boost::get(boost::edge_residual_capacity, m_auxGraph);
    }
                           
    template <typename LP>
    bool feasibleSolution(const LP & lp) {
        fillAuxiliaryDigraph(lp);
        return !findViolatedConstraint();
    }
    
    template <typename LP>
    void addViolatedConstraint(LP & lp) {
        
    }

private:
    typedef boost::adjacency_list_traits < boost::vecS, boost::vecS, boost::directedS > Traits;
    typedef Traits::edge_descriptor AuxEdge;
    typedef Traits::vertex_descriptor AuxVertex;
    typedef boost::adjacency_list < boost::vecS, boost::vecS, boost::directedS,
                                    boost::no_property,
                                    boost::property < boost::edge_capacity_t, double,
                                        boost::property < boost::edge_residual_capacity_t, double,
                                            boost::property < boost::edge_reverse_t, AuxEdge >
                                                        >
                                                    >
                                  > AuxGraph;
                                  
    template <typename LP>
    void fillAuxiliaryDigraph(const LP & lp) {
        m_auxGraph.clear();
        AuxGraph tmpGraph(boost::num_vertices(*m_g));
        std::swap(tmpGraph, m_auxGraph);
        
        for (const std::pair<Edge, std::string> & e : (*m_edgeNameMap)) {
            int colIdx = lp.getColByName(e.second);
            if (0 != colIdx) {
                double colVal = lp.getColPrim(colIdx) / 2;
                
                if (!utils::Compare<double>::e(colVal, 0)) {
                    Vertex u = source(e.first, *m_g);
                    Vertex v = target(e.first, *m_g);
                    addEdge(u, v, colVal);
                    addEdge(v, u, colVal);
                }
            }
        }
        
        m_src = boost::add_vertex(m_auxGraph);
        m_trg = boost::add_vertex(m_auxGraph);
        
        for (const Vertex & v : (*m_vertexList)) {
            addEdge(v, m_trg, 1);
            addEdge(m_src, v, degreeOf(v, lp) / 2);
        }
    }
    
    template <typename SrcVertex, typename TrgVertex>
    void addEdge(const SrcVertex & vSrc, const TrgVertex & vTrg, double cap) {
        bool b, bRev;
        AuxEdge e, eRev;
        
        std::tie(e, b) = boost::add_edge(vSrc, vTrg, m_auxGraph);
        std::tie(eRev, bRev) = boost::add_edge(vTrg, vSrc, m_auxGraph);
        
        assert(b && bRev);
        
        m_cap[e] = cap;
        m_cap[eRev] = 0;
        
        m_rev[e] = eRev;
        m_rev[eRev] = e;
    }
    
    template <typename LP>
    double degreeOf(const Vertex & v, const LP & lp) {
        double res = 0;
        auto adjVertices = boost::adjacent_vertices(v, *m_g);
            
        for(const Vertex & u : utils::make_range(adjVertices.first, adjVertices.second)) {
            bool b; Edge e;
            std::tie(e, b) = boost::edge(v, u, *m_g);
            
            if (b) {
                int colIdx = lp.getColByName(m_edgeNameMap->find(e)->second);
                if (0 != colIdx) {
                    res += lp.getColPrim(colIdx);
                }
            }
        }
        return res;
    }
    
    bool findViolatedConstraint() {
        return false;
    }
  
    const Graph *               m_g;
    const EdgeNameMap *         m_edgeNameMap;
    const VertexList *          m_vertexList;
    
    AuxGraph    m_auxGraph;
    Vertex      m_src;
    Vertex      m_trg;
    
    boost::property_map < AuxGraph, boost::edge_capacity_t >::type              m_cap;
    boost::property_map < AuxGraph, boost::edge_reverse_t >::type               m_rev;
    boost::property_map < AuxGraph, boost::edge_residual_capacity_t >::type     m_resCap;
};

} //ir
} //paal
#endif /* BOUNDED_DEGREE_MST_ORACLE_HPP */
