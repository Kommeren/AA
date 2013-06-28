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
#include <boost/bimap.hpp>

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
    
    typedef boost::bimap<Edge, ColId> EdgeMap;
    typedef std::vector<Vertex> VertexList;
    
    BoundedDegreeMSTOracle(const Graph & g, const EdgeMap & edgeMap,
                           const VertexList & vertexList, const utils::Compare<double> & compare)
                           : m_g(g), m_edgeMap(edgeMap), m_vertexList(vertexList), m_compare(compare)
    { }

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
        
        if (m_oracleComponents.initialTest(*this)) {
            return true;
        }
        else {
            return !m_oracleComponents.findViolated(*this, boost::num_vertices(m_g));
        }
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
        
        for (auto const & e : m_edgeMap) {
            const Vertex & u = boost::source(e.left, m_g);
            const Vertex & v = boost::target(e.left, m_g);
            
            if (m_violatingSet[u] && m_violatingSet[v]) {
                ColId colIdx = e.right;
                lp.addNewRowCoef(colIdx);
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
        
        std::pair<bool, double> violation;
                
        for (const Vertex & trg : utils::make_range(vertices)) {
            if (src != trg && trg != m_src && trg != m_trg) {
                violation = checkViolationGreaterThan(src, trg);
                if (violation.first) {
                    return true;
                }
                
                violation = checkViolationGreaterThan(trg, src);
                if (violation.first) {
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
        
        bool violatedConstraintFound = false;
        double maximumViolation = 0;
        std::pair<bool, double> violation;
        
        for (const Vertex & trg : utils::make_range(vertices)) {
            if (src != trg && trg != m_src && trg != m_trg) {
                violation = checkViolationGreaterThan(src, trg, maximumViolation);
                maximumViolation = std::max(maximumViolation, violation.second);
                if (violation.first) {
                    violatedConstraintFound = true;
                }
                
                violation = checkViolationGreaterThan(trg, src, maximumViolation);
                maximumViolation = std::max(maximumViolation, violation.second);
                if (violation.first) {
                    violatedConstraintFound = true;
                }
            }
        }
        
        return violatedConstraintFound;
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
        int numVertices(boost::num_vertices(m_g));
        
        m_auxGraph.clear();
        m_srcToV.resize(numVertices);
        m_vToTrg.resize(numVertices);
        
        AuxGraph tmpGraph(numVertices);
        std::swap(tmpGraph, m_auxGraph);
        m_cap = boost::get(boost::edge_capacity, m_auxGraph);
        m_rev = boost::get(boost::edge_reverse, m_auxGraph);
        m_resCap = boost::get(boost::edge_residual_capacity, m_auxGraph);
        
        for (auto const & e : m_edgeMap) {
            ColId colIdx = e.right;
            double colVal = lp.getColPrim(colIdx) / 2;
            
            if (!m_compare.e(colVal, 0)) {
                Vertex u = source(e.left, m_g);
                Vertex v = target(e.left, m_g);
                addEdge(u, v, colVal);
            }
        }
        
        m_src = boost::add_vertex(m_auxGraph);
        m_trg = boost::add_vertex(m_auxGraph);
        
        for (const Vertex & v : m_vertexList) {
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
        
        boost::put(m_cap, e, cap);
        if (noRev) {
            boost::put(m_cap, eRev, 0);
        }
        else {
            boost::put(m_cap, eRev, cap);
        }
        
        boost::put(m_rev, e, eRev);
        boost::put(m_rev, eRev, e);
        
        return e;
    }
   


    //CR taka funkcja juz chyba jest w GLP i nazywa sie getColSum/getRowSum
    // TODO: getRowSum robi troche co innego, ale mozne dodac do GLP ogolna funkcje robiaca to co degreeOf
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
        auto adjEdges = boost::out_edges(v, m_g);
            
        for (Edge e : utils::make_range(adjEdges)) {
            auto i = m_edgeMap.left.find(e);
            if(i != m_edgeMap.left.end()) {
                res += lp.getColPrim(i->second);
            }
        }
        return res;
    }
    
    /**
     * @brief finds the most violated set of vertices containing \c src and avoiding \c trg and saves it if the violation is greater than \c minViolation
     * @param src vertex to be contained in the violating set
     * @param trg vertex not to be contained in the violating set
     * @param minViolation minimum violation that a set should have to be saved
     * @return a pair of bool (if a violated set was found) and the violation of the found set
     */
    std::pair<bool, double> checkViolationGreaterThan(const Vertex & src, const Vertex & trg, double minViolation = 0) {
        int numVertices(boost::num_vertices(m_g));
        double origVal = boost::get(m_cap, m_srcToV[src]);
        bool violated = false;

        boost::put(m_cap, m_srcToV[src], numVertices);
        // capacity of srcToV[trg] does not change
        boost::put(m_cap, m_vToTrg[src], 0);
        boost::put(m_cap, m_vToTrg[trg], numVertices);
        
        // TODO better flow algorithm
        double minCut = boost::boykov_kolmogorov_max_flow(m_auxGraph, m_src, m_trg);
        double violation = numVertices - 1 - minCut;
        
        if (violation > minViolation && !m_compare.e(violation, 0)) {
            violated = true;
            m_violatingSetSize = 0;
            
            auto vertices = boost::vertices(m_auxGraph);
            auto colors = boost::get(boost::vertex_color, m_auxGraph);
            auto srcColor = boost::get(colors, m_src);
            for (const Vertex & v : utils::make_range(vertices.first, vertices.second)) {
                if (v != m_src && v != m_trg && boost::get(colors, v) == srcColor) {
                    m_violatingSet[v] = true;
                    ++m_violatingSetSize;
                }
                else {
                    m_violatingSet[v] = false;
                }
            }
        }
        
        // reset the original values for the capacities
        boost::put(m_cap, m_srcToV[src], origVal);
        // capacity of srcToV[trg] does not change
        boost::put(m_cap, m_vToTrg[src], 1);
        boost::put(m_cap, m_vToTrg[trg], 1);
        
        return std::make_pair(violated, violation);
    }
    
    
    OracleComponents    m_oracleComponents;
    
    const Graph &               m_g;
    const EdgeMap &         m_edgeMap;
    const VertexList &          m_vertexList;
    const utils::Compare<double> & m_compare;
    
    AuxGraph    m_auxGraph;
    Vertex      m_src;
    Vertex      m_trg;
    
    AuxEdgeList       m_srcToV;
    AuxEdgeList       m_vToTrg;
    
    ViolatingSet        m_violatingSet;
    int                 m_violatingSetSize;
    
    boost::property_map < AuxGraph, boost::edge_capacity_t >::type              m_cap;
    boost::property_map < AuxGraph, boost::edge_reverse_t >::type               m_rev;
    boost::property_map < AuxGraph, boost::edge_residual_capacity_t >::type     m_resCap;
};

} //ir
} //paal
#endif /* BOUNDED_DEGREE_MST_ORACLE_HPP */
