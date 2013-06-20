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
#include <boost/graph/boykov_kolmogorov_max_flow.hpp>

#include "paal/utils/double_rounding.hpp"


namespace paal {
namespace ir {

/**
 * @class BoundedDegreeMSTOracle
 * @brief this is a separation oracle for the row generation in the bounded degree minimum spanning tree problem.
 *
 * @tparam Graph input graph, has to be a model of boost::Graph
 */
template <typename Graph, typename OracleComponents>
class BoundedDegreeMSTOracle {
public:
    typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;
    typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
    
    typedef std::map<Edge, std::string> EdgeNameMap;
    typedef std::vector<Vertex> VertexList;
    
    BoundedDegreeMSTOracle() : m_g(0), m_edgeNameMap(0)
    { }
    
    /**
     * @brief initializes the pointers to problem instance data structures
     * @param g pointer to the input graph
     * @param vertexList pointer to the input graph vertices list
     * @param edgeMap pointer to the input graph edge names map
     * @param epsilon tolerance for double comparison
     */
    void init(const Graph * g, const VertexList * vertexList, const EdgeNameMap * edgeMap,
              const double & epsilon) {
        m_g = g;
        m_vertexList = vertexList;
        m_edgeNameMap = edgeMap;
        EPSILON = epsilon;
    }

    /**
     * @brief checks if the current LP solution is feasible
     * @param lp LP object
     * @return true iff the current LP solution is feasible
     *
     * @tparam LP
     */
    template <typename LP>
    bool feasibleSolution(const LP & lp) {
        fillAuxiliaryDigraph(lp);
        //CR jak to sie powiedzie to chyba powinnismy cos zwrocic
        m_oracleComponents.initialTest(*this);
        return !m_oracleComponents.findViolated(*this, boost::num_vertices(*m_g));
    }
    
    /**
     * @brief adds a violated constraint to the LP
     * @param lp LP object
     *
     * @tparam LP
     */
    template <typename LP>
    void addViolatedConstraint(LP & lp) {
        lp.addRow(UP, 0, m_violatingSetSize - 1);
        
        for (const std::pair<Edge, std::string> & e : *m_edgeNameMap) {
            const Vertex & u = boost::source(e.first, *m_g);
            const Vertex & v = boost::target(e.first, *m_g);
            
            if (m_violatingSet[u] && m_violatingSet[v]) {
                int colIdx = lp.getColByName(e.second);
                
                if (0 != colIdx) {
                    lp.addNewRowCoef(colIdx);
                }
            }
        }
        
        lp.loadNewRow();
    }
    
    
    /**
     * @brief finds any violated constraint
     * @param srcVertexIndex index of the source vertex
     * @return true iff a violated consrtaint was found
     */
    bool findAnyViolatedConstraint(int srcVertexIndex) {
        auto vertices = boost::vertices(m_auxGraph);
        const Vertex & src = boost::vertex(srcVertexIndex, m_auxGraph);
        assert(src != m_src && src != m_trg);
        
        m_violatedConstraintFound = false;
        m_maximumViolation = 0;
        
        for (const Vertex & trg : utils::make_range(vertices.first, vertices.second)) {
            if (src != trg && trg != m_src && trg != m_trg) {
                checkViolation(src, trg);
                if (m_violatedConstraintFound) {
                    return true;
                }
                
                checkViolation(trg, src);
                if (m_violatedConstraintFound) {
                    return true;
                }
            }
        }
        
        return false;
    }
    
    /**
     * @brief finds the most violated constraint
     * @return true iff a violated consrtaint was found
     */
    bool findMostViolatedConstraint() {
        auto vertices = boost::vertices(m_auxGraph);
        const Vertex & src = *(vertices.first);
        assert(src != m_src && src != m_trg);
        
        m_violatedConstraintFound = false;
        m_maximumViolation = 0;
        
        for (const Vertex & trg : utils::make_range(vertices.first, vertices.second)) {
            if (src != trg && trg != m_src && trg != m_trg) {
                checkViolation(src, trg);
                checkViolation(trg, src);
            }
        }
        
        return m_violatedConstraintFound;
    }

private:
    typedef boost::adjacency_list_traits < boost::vecS, boost::vecS, boost::directedS > Traits;
    typedef Traits::edge_descriptor AuxEdge;
    typedef Traits::vertex_descriptor AuxVertex;
    typedef boost::adjacency_list < boost::vecS, boost::vecS, boost::directedS,
                                    boost::property < boost::vertex_color_t, boost::default_color_type,
                                        boost::property < boost::vertex_distance_t, long,
                                            boost::property < boost::vertex_predecessor_t, AuxEdge >
                                                        >
                                                    >,
                                    boost::property < boost::edge_capacity_t, double,
                                        boost::property < boost::edge_residual_capacity_t, double,
                                            boost::property < boost::edge_reverse_t, AuxEdge >
                                                        >
                                                    >
                                  > AuxGraph;
    typedef std::vector < AuxEdge > AuxEdgeList;
    typedef std::map < AuxVertex, bool > ViolatingSet;
    
    /**
     * @brief creates the auxiliary directed graph used for feasibility testing
     * @param lp LP object
     *
     * @tparam LP
     */
    template <typename LP>
    void fillAuxiliaryDigraph(const LP & lp) {
        int numVertices(boost::num_vertices(*m_g));
        
        m_auxGraph.clear();
        m_srcToV.resize(numVertices);
        m_vToTrg.resize(numVertices);
        
        AuxGraph tmpGraph(numVertices);
        std::swap(tmpGraph, m_auxGraph);
        m_cap = boost::get(boost::edge_capacity, m_auxGraph);
        m_rev = boost::get(boost::edge_reverse, m_auxGraph);
        m_resCap = boost::get(boost::edge_residual_capacity, m_auxGraph);
        
        for (const std::pair<Edge, std::string> & e : (*m_edgeNameMap)) {
            int colIdx = lp.getColByName(e.second);
            if (0 != colIdx) {
                double colVal = lp.getColPrim(colIdx) / 2;
                
                if (!utils::Compare<double>::e(colVal, 0, EPSILON)) {
                    Vertex u = source(e.first, *m_g);
                    Vertex v = target(e.first, *m_g);
                    addEdge(u, v, colVal);
                    //addEdge(v, u, colVal);
                }
            }
        }
        
        m_src = boost::add_vertex(m_auxGraph);
        m_trg = boost::add_vertex(m_auxGraph);
        
        for (const Vertex & v : (*m_vertexList)) {
            m_srcToV[v] = addEdge(m_src, v, degreeOf(v, lp) / 2, true);
            m_vToTrg[v] = addEdge(v, m_trg, 1, true);
        }
    }
    
    /**
     * @brief adds an edge to the auxiliary graph
     * @param vSrc source vertex of for the added edge
     * @param vTrg target vertex of for the added edge
     * @param cap capacity of the added edge
     * @param noRev if the reverse edge should have zero capacity
     * @return created edge of the auxiliary graph
     *
     * @tparam SrcVertex
     * @tparam TrgVertex
     */
    template <typename SrcVertex, typename TrgVertex>
    AuxEdge addEdge(const SrcVertex & vSrc, const TrgVertex & vTrg, double cap, bool noRev = false) {
        bool b, bRev;
        AuxEdge e, eRev;
        
        std::tie(e, b) = boost::add_edge(vSrc, vTrg, m_auxGraph);
        std::tie(eRev, bRev) = boost::add_edge(vTrg, vSrc, m_auxGraph);
        
        assert(b && bRev);
        
        //CR taka ogolna uwaga do boostowych map
        //to to, zeby do nich uzywac funkcji get i put
        //to co ty tutaj  robisz raczej zawsze bedzie dzialac
        //ale jezeli otrzymalbys graf z zewnatrz to nie mozesz zalozyc, ze property map bedzie mialo operator[]
        //perwnie lepiej po prostu zawsze uzywac get i put
        m_cap[e] = cap;
        if (noRev) {
            m_cap[eRev] = 0;
        }
        else {
            m_cap[eRev] = cap;
        }
        
        m_rev[e] = eRev;
        m_rev[eRev] = e;
        
        return e;
    }
   


    //CR taka funkcja juz chyba jest w GLP i nazywa sie getColSum/getRowSum
    /**
     * @brief calculates the sum of the variables for edges incident with a given vertex
     * @param v vertex
     * @param lp LP object
     * @return sum of variables for edges incident with given vertex
     *
     * @tparam SrcVertex
     * @tparam TrgVertex
     */
    template <typename LP>
    double degreeOf(const Vertex & v, const LP & lp) {
        double res = 0;
        auto adjVertices = boost::adjacent_vertices(v, *m_g);
            
        for (const Vertex & u : utils::make_range(adjVertices.first, adjVertices.second)) {
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
    

    //CR checkViolation moglo by zwracac boola i wtedy nie byla by potrzebna skaldowa m_violatedConstraintFound
    //takie sterowanie przez zmienna skladowa jest troche nieczytelne
    //
    //podobnie m_maximumViolation mogloby być np. argumentem tej funkcji, 
    //moglaby sie ona nazywac na przyklad checkViolationGreaterThan, argument moglby miec wartosc domyslna.
    /**
     * @brief finds the most violated set of vertices containing \c src and avoiding \c trg
     * @param src vertex to be contained in the violating set
     * @param trg vertex not to be contained in the violating set
     */
    void checkViolation(const Vertex & src, const Vertex & trg) {
        int numVertices(boost::num_vertices(*m_g));
        double origVal = m_cap[m_srcToV[src]];

        m_cap[m_srcToV[src]] = numVertices;
        // capacity of srcToV[trg] does not change
        m_cap[m_vToTrg[src]] = 0;
        m_cap[m_vToTrg[trg]] = numVertices;
        
        // TODO better flow algorithm
        double minCut = boost::boykov_kolmogorov_max_flow(m_auxGraph, m_src, m_trg);
        double violation = numVertices - 1 - minCut;
        
        if (violation > m_maximumViolation && !utils::Compare<double>::e(violation, 0, EPSILON)) {
            m_maximumViolation = violation;
            m_violatedConstraintFound = true;
            m_violatingSetSize = 0;
            
            auto vertices = boost::vertices(m_auxGraph);
            auto colors = boost::get(boost::vertex_color, m_auxGraph);
            auto srcColor = colors[m_src];
            for (const Vertex & v : utils::make_range(vertices.first, vertices.second)) {
                if (v != m_src && v != m_trg && colors[v] == srcColor) {
                    m_violatingSet[v] = true;
                    ++m_violatingSetSize;
                }
                else {
                    m_violatingSet[v] = false;
                }
            }
        }
        
        // reset the original values for the capacities
        m_cap[m_srcToV[src]] = origVal;
        // capacity of srcToV[trg] does not change
        m_cap[m_vToTrg[src]] = 1;
        m_cap[m_vToTrg[trg]] = 1;
    }
    
//CR to private nie jest potrzebne
//
//jak chcesz zaznaczyc, ze teraz sa składowe to lepiej zrobidc komentarz
private:
    OracleComponents    m_oracleComponents;
    
    const Graph *               m_g;
    const EdgeNameMap *         m_edgeNameMap;
    const VertexList *          m_vertexList;
    
    AuxGraph    m_auxGraph;
    Vertex      m_src;
    Vertex      m_trg;
    
    AuxEdgeList       m_srcToV;
    AuxEdgeList       m_vToTrg;
    
    ViolatingSet        m_violatingSet;
    int                 m_violatingSetSize;
    
    bool                m_violatedConstraintFound;
    double              m_maximumViolation;
    
    boost::property_map < AuxGraph, boost::edge_capacity_t >::type              m_cap;
    boost::property_map < AuxGraph, boost::edge_reverse_t >::type               m_rev;
    boost::property_map < AuxGraph, boost::edge_residual_capacity_t >::type     m_resCap;
    
    double EPSILON;
};

} //ir
} //paal
#endif /* BOUNDED_DEGREE_MST_ORACLE_HPP */
