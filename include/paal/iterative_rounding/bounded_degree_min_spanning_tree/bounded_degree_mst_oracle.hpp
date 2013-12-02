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

#include "paal/utils/floating.hpp"
#include "paal/iterative_rounding/bounded_degree_min_spanning_tree/bounded_degree_mst_oracle_components.hpp"


namespace paal {
namespace ir {

/**
 * @class BoundedDegreeMSTOracle
 * @brief Separation oracle for the row generation in the bounded degree minimum spanning tree problem.
 *
 * @tparam Graph input graph, has to be a model of boost::Graph
 * @tparam OracleComponents
 */
template <typename Graph, typename OracleComponents = BoundedDegreeMSTOracleComponents<> >
class BoundedDegreeMSTOracle {
public:
    typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;
    typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;

    BoundedDegreeMSTOracle(const Graph & g) : m_g(g)
    { }

    /**
     * Checks if the current LP solution is a feasible solution of the problem.
     */
    template <typename Problem, typename LP>
    bool feasibleSolution(const Problem &problem, const LP & lp) {
        fillAuxiliaryDigraph(problem, lp);

        if (m_oracleComponents.initialTest(*this)) {
            return true;
        }
        else {
            return !m_oracleComponents.findViolated(problem, *this, num_vertices(m_g));
        }
    }

    /**
     * Adds a violated constraint to the LP.
     */
    template <typename Problem, typename LP>
    void addViolatedConstraint(Problem & problem, LP & lp) {
        lp.addRow(lp::UP, 0, m_violatingSetSize - 1);

        for (auto const & e : problem.getEdgeMap().right) {
            const Vertex & u = source(e.second, m_g);
            const Vertex & v = target(e.second, m_g);

            if (m_violatingSet[u] && m_violatingSet[v]) {
                lp.addNewRowCoef(e.first);
            }
        }

        lp.loadNewRow();
    }


    /**
     * Finds any violated constraint and saves it or decides that no constraint is violated.
     *
     * @param problem Problem object
     * @param srcVertexIndex index of the vertex from which we begin the search
     * @return true iff a violated constraint was found
     */
    template <typename Problem>
    bool findAnyViolatedConstraint(Problem & problem, int srcVertexIndex = 0) {
        auto vert = vertices(m_auxGraph);
        std::advance(vert.first, srcVertexIndex);
        auto src = *(vert.first);
        assert(src != m_src && src != m_trg);

        for (const Vertex & trg : boost::make_iterator_range(vertices(m_auxGraph))) {
            if (src != trg && trg != m_src && trg != m_trg) {
                if (problem.getCompare().g(
                        checkViolationGreaterThan(problem, src, trg), 0)) {
                    return true;
                }

                if (problem.getCompare().g(
                        checkViolationGreaterThan(problem, trg, src), 0)) {
                    return true;
                }
            }
        }

        return false;
    }

    /**
     * Finds the most violated constraint and saves it or decides that no constraint is violated.
     *
     * @param problem Problem object
     * @return true iff a violated constraint was found
     */
    template <typename Problem>
    bool findMostViolatedConstraint(Problem & problem) {
        auto graphVertices = vertices(m_auxGraph);
        auto src = *(graphVertices.first);
        assert(src != m_src && src != m_trg);

        double maximumViolation = 0;

        for (const Vertex & trg : boost::make_iterator_range(graphVertices)) {
            if (src != trg && trg != m_src && trg != m_trg) {
                maximumViolation = std::max(maximumViolation,
                    checkViolationGreaterThan(problem, src, trg, maximumViolation));
                maximumViolation = std::max(maximumViolation,
                    checkViolationGreaterThan(problem, trg, src, maximumViolation));
            }
        }

        return problem.getCompare().g(maximumViolation, 0);
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
    typedef std::unordered_map < AuxVertex, bool > ViolatingSet;

    /**
     * Creates the auxiliary directed graph used for feasibility testing.
     */
    template <typename Problem, typename LP>
    void fillAuxiliaryDigraph(Problem & problem, const LP & lp) {
        m_auxGraph = AuxGraph(num_vertices(m_g));
        m_cap = get(boost::edge_capacity, m_auxGraph);
        m_rev = get(boost::edge_reverse, m_auxGraph);

        m_srcToV.resize(num_vertices(m_g));
        m_vToTrg.resize(num_vertices(m_g));

        for (auto const & e : problem.getEdgeMap().right) {
            lp::ColId colIdx = e.first;
            double colVal = lp.getColPrim(colIdx) / 2;

            if (!problem.getCompare().e(colVal, 0)) {
                Vertex u = source(e.second, m_g);
                Vertex v = target(e.second, m_g);
                addEdge(u, v, colVal);
            }
        }

        m_src = add_vertex(m_auxGraph);
        m_trg = add_vertex(m_auxGraph);

        for (const Vertex & v : problem.getVertices()) {
            m_srcToV[v] = addEdge(m_src, v, degreeOf(problem, v, lp) / 2, true);
            m_vToTrg[v] = addEdge(v, m_trg, 1, true);
        }
    }

    /**
     * Adds an edge to the auxiliary graph.
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

        std::tie(e, b) = add_edge(vSrc, vTrg, m_auxGraph);
        std::tie(eRev, bRev) = add_edge(vTrg, vSrc, m_auxGraph);

        assert(b && bRev);

        put(m_cap, e, cap);
        if (noRev) {
            put(m_cap, eRev, 0);
        }
        else {
            put(m_cap, eRev, cap);
        }

        put(m_rev, e, eRev);
        put(m_rev, eRev, e);

        return e;
    }


    /**
     * Calculates the sum of the variables for edges incident with a given vertex.
     */
    template <typename Problem, typename LP>
    double degreeOf(Problem & problem, const Vertex & v, const LP & lp) {
        double res = 0;
        auto adjEdges = out_edges(v, m_g);

        for (Edge e : boost::make_iterator_range(adjEdges)) {
            auto colId = problem.edgeToCol(e);
            if (colId) {
                res += lp.getColPrim(*colId);
            }
        }
        return res;
    }

    /**
     * Finds the most violated set of vertices containing \c src and not containing
     * \c trg and saves it if the violation is greater than \c minViolation
     * @param src vertex to be contained in the violating set
     * @param trg vertex not to be contained in the violating set
     * @param minViolation minimum violation that a set should have to be saved
     * @return violation of the found set
     */
    template <typename Problem>
    double checkViolationGreaterThan(Problem & problem, Vertex src, Vertex trg,
                double minViolation = 0.) {
        int numVertices(num_vertices(m_g));
        double origVal = get(m_cap, m_srcToV[src]);

        put(m_cap, m_srcToV[src], numVertices);
        // capacity of srcToV[trg] does not change
        put(m_cap, m_vToTrg[src], 0);
        put(m_cap, m_vToTrg[trg], numVertices);

        // TODO better flow algorithm
        double minCut = boost::boykov_kolmogorov_max_flow(m_auxGraph, m_src, m_trg);
        double violation = numVertices - 1 - minCut;

        if (problem.getCompare().g(violation, minViolation)) {
            m_violatingSetSize = 0;

            auto colors = get(boost::vertex_color, m_auxGraph);
            auto srcColor = get(colors, m_src);
            for (const Vertex & v : boost::make_iterator_range(vertices(m_auxGraph))) {
                if (v != m_src && v != m_trg && get(colors, v) == srcColor) {
                    m_violatingSet[v] = true;
                    ++m_violatingSetSize;
                }
                else {
                    m_violatingSet[v] = false;
                }
            }
        }

        // reset the original values for the capacities
        put(m_cap, m_srcToV[src], origVal);
        // capacity of srcToV[trg] does not change
        put(m_cap, m_vToTrg[src], 1);
        put(m_cap, m_vToTrg[trg], 1);

        return violation;
    }


    OracleComponents m_oracleComponents;

    const Graph & m_g;

    AuxGraph m_auxGraph;
    Vertex   m_src;
    Vertex   m_trg;

    AuxEdgeList  m_srcToV;
    AuxEdgeList  m_vToTrg;

    ViolatingSet m_violatingSet;
    int          m_violatingSetSize;

    boost::property_map < AuxGraph, boost::edge_capacity_t >::type m_cap;
    boost::property_map < AuxGraph, boost::edge_reverse_t >::type  m_rev;
};


/**
 * @brief Creates a BoundedDegreeMSTOracle object.
 *
 * @tparam OracleComponents
 * @tparam Graph
 * @param g
 *
 * @return BoundedDegreeMSTOracle object
 */
template <typename OracleComponents = BoundedDegreeMSTOracleComponents<>, typename Graph>
BoundedDegreeMSTOracle<Graph, OracleComponents>
make_BoundedDegreeMSTOracle(const Graph & g) {
    return  BoundedDegreeMSTOracle<Graph, OracleComponents>(g);
}

} //ir
} //paal
#endif /* BOUNDED_DEGREE_MST_ORACLE_HPP */
