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
#include <unordered_set>

#include "paal/data_structures/components/components.hpp"
#include "paal/lp/lp_row_generation.hpp"
#include "paal/lp/separation_oracle_components.hpp"
#include "paal/utils/floating.hpp"
#include "paal/utils/functors.hpp"


namespace paal {
namespace ir {

class FindViolated;

/**
 * @brief Components of the separation oracle for the bounded degree minimum spanning tree problem.
 */
template <typename... Args>
    using BoundedDegreeMSTOracleComponents =
        data_structures::Components<
        data_structures::NameWithDefault<FindViolated, lp::FindRandViolated>>::type<Args...>;


/**
 * @class BoundedDegreeMSTOracle
 * @brief Separation oracle for the row generation in the bounded degree minimum spanning tree problem.
 *
 * @tparam OracleComponents
 */
template <typename OracleComponents = BoundedDegreeMSTOracleComponents<> >
class BoundedDegreeMSTOracle {
public:
    /**
     * Checks if the current LP solution is a feasible solution of the problem.
     */
    template <typename Problem, typename LP>
    bool feasibleSolution(const Problem & problem, const LP & lp) {
        fillAuxiliaryDigraph(problem, lp);

        return !m_oracleComponents.template call<FindViolated>(
                    problem, *this, num_vertices(problem.getGraph()));
    }

    /**
     * Adds a violated constraint to the LP.
     */
    template <typename Problem, typename LP>
    void addViolatedConstraint(const Problem & problem, LP & lp) {
        const auto & g = problem.getGraph();
        lp.addRow(lp::UP, 0, m_violatingSet.size() - 1);

        for (auto const & e : problem.getEdgeMap().right) {
            auto u = source(e.second, g);
            auto v = target(e.second, g);

            if ((m_violatingSet.find(u) != m_violatingSet.end())
                && (m_violatingSet.find(v) != m_violatingSet.end())) {
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
    bool findAnyViolatedConstraint(const Problem & problem, int srcVertexIndex = 0) {
        auto vert = vertices(m_auxGraph);
        std::advance(vert.first, srcVertexIndex);
        auto src = *(vert.first);
        assert(src != m_src && src != m_trg);

        for (auto trg : boost::make_iterator_range(vertices(m_auxGraph))) {
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
    bool findMostViolatedConstraint(const Problem & problem) {
        auto graphVertices = vertices(m_auxGraph);
        auto src = *(graphVertices.first);
        assert(src != m_src && src != m_trg);

        double maximumViolation = 0;

        for (auto trg : boost::make_iterator_range(graphVertices)) {
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
    typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> Traits;
    typedef Traits::edge_descriptor AuxEdge;
    typedef Traits::vertex_descriptor AuxVertex;
    typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS,
                                  boost::property<boost::vertex_color_t, boost::default_color_type,
                                      boost::property<boost::vertex_distance_t, long,
                                          boost::property<boost::vertex_predecessor_t, AuxEdge>>>,
                                  boost::property<boost::edge_capacity_t, double,
                                      boost::property<boost::edge_residual_capacity_t, double,
                                          boost::property<boost::edge_reverse_t, AuxEdge>>>
                                 > AuxGraph;
    typedef boost::property_map<AuxGraph, boost::edge_capacity_t>::type AuxEdgeCapacity;
    typedef boost::property_map<AuxGraph, boost::edge_reverse_t>::type  AuxEdgeReverse;
    typedef std::vector<AuxEdge> AuxEdgeList;
    typedef std::unordered_set<AuxVertex> ViolatingSet;

    /**
     * Creates the auxiliary directed graph used for feasibility testing.
     */
    template <typename Problem, typename LP>
    void fillAuxiliaryDigraph(const Problem & problem, const LP & lp) {
        const auto & g = problem.getGraph();
        m_auxGraph = AuxGraph(num_vertices(g));
        m_cap = get(boost::edge_capacity, m_auxGraph);
        m_rev = get(boost::edge_reverse, m_auxGraph);

        m_srcToV.resize(num_vertices(g));
        m_vToTrg.resize(num_vertices(g));

        for (auto const & e : problem.getEdgeMap().right) {
            lp::ColId colIdx = e.first;
            double colVal = lp.getColPrim(colIdx) / 2;

            if (!problem.getCompare().e(colVal, 0)) {
                auto u = source(e.second, g);
                auto v = target(e.second, g);
                addEdge(u, v, colVal);
            }
        }

        m_src = add_vertex(m_auxGraph);
        m_trg = add_vertex(m_auxGraph);

        for (auto v : boost::make_iterator_range(vertices(g))) {
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
    template <typename Problem, typename LP, typename Vertex>
    double degreeOf(const Problem & problem, const Vertex & v, const LP & lp) {
        double res = 0;
        auto adjEdges = out_edges(v, problem.getGraph());

        for (auto e : boost::make_iterator_range(adjEdges)) {
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
    double checkViolationGreaterThan(const Problem & problem, AuxVertex src, AuxVertex trg,
                double minViolation = 0.) {
        int numVertices(num_vertices(problem.getGraph()));
        double origVal = get(m_cap, m_srcToV[src]);

        put(m_cap, m_srcToV[src], numVertices);
        // capacity of srcToV[trg] does not change
        put(m_cap, m_vToTrg[src], 0);
        put(m_cap, m_vToTrg[trg], numVertices);

        // TODO better flow algorithm
        double minCut = boost::boykov_kolmogorov_max_flow(m_auxGraph, m_src, m_trg);
        double violation = numVertices - 1 - minCut;

        if (problem.getCompare().g(violation, minViolation)) {
            m_violatingSet.clear();

            auto colors = get(boost::vertex_color, m_auxGraph);
            auto srcColor = get(colors, m_src);
            assert(srcColor != get(colors, m_trg));
            for (auto v : boost::make_iterator_range(vertices(m_auxGraph))) {
                if (v != m_src && v != m_trg && get(colors, v) == srcColor) {
                    m_violatingSet.insert(v);
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

    AuxGraph m_auxGraph;
    AuxVertex   m_src;
    AuxVertex   m_trg;

    AuxEdgeList  m_srcToV;
    AuxEdgeList  m_vToTrg;

    ViolatingSet m_violatingSet;

    AuxEdgeCapacity m_cap;
    AuxEdgeReverse  m_rev;
};


} //ir
} //paal
#endif /* BOUNDED_DEGREE_MST_ORACLE_HPP */
