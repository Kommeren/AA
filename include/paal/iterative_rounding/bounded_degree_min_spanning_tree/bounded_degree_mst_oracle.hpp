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
#include <boost/optional.hpp>

#include <unordered_set>
#include <vector>


namespace paal {
namespace ir {


/**
 * @class BDMSTViolationChecker
 * @brief Violations checker for the separation oracle
 *      in the bounded degree minimum spanning tree problem.
 */
class BDMSTViolationChecker {
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
    typedef boost::optional<double> Violation;

public:
    typedef std::pair<AuxVertex, AuxVertex> Candidate;
    typedef std::vector<Candidate> CandidateList;

    /**
     * Returns an iterator range of violated constraint candidates.
     */
    template <typename Problem, typename LP>
    const CandidateList & getViolationCandidates(const Problem & problem, const LP & lp) {
        fillAuxiliaryDigraph(problem, lp);
        initializeCandidates();
        return m_candidateList;
    }

    /**
     * Checks if the given constraint candidate is violated an if it is,
     * returns the violation value and violated constraint ID.
     */
    template <typename Problem>
    Violation checkViolation(Candidate candidate, const Problem & problem) {
        double violation = checkMinCut(candidate.first, candidate.second, problem.getCompare());
        if (problem.getCompare().g(violation, 0)) {
            return Violation(violation);
        }
        else {
            return Violation();
        }
    }

    /**
     * Adds a violated constraint to the LP.
     */
    template <typename Problem, typename LP>
    void addViolatedConstraint(Candidate violatingPair, const Problem & problem, LP & lp) {
        if (violatingPair != m_violatingPair) {
            checkMinCut(violatingPair.first, violatingPair.second, problem.getCompare());
        }

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

private:

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
     * Initializes the list of cut candidates.
     */
    void initializeCandidates() {
        auto src = *(std::next(vertices(m_auxGraph).first, rand() % (num_vertices(m_auxGraph) - 2)));
        assert(src != m_src && src != m_trg);
        m_candidateList.clear();
        for (AuxVertex v : boost::make_iterator_range(vertices(m_auxGraph))) {
            if (v != src && v != m_src && v != m_trg) {
                m_candidateList.push_back(std::make_pair(src, v));
                m_candidateList.push_back(std::make_pair(v, src));
            }
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
     * \c trg and saves it if a violation is found.
     * @param src vertex to be contained in the violating set
     * @param trg vertex not to be contained in the violating set
     * @param compare double comparison object
     * @return violation of the found set
     */
    template <typename Compare>
    double checkMinCut(AuxVertex src, AuxVertex trg, Compare compare) {
        int numVertices(num_vertices(m_auxGraph) - 2);
        double origVal = get(m_cap, m_srcToV[src]);

        put(m_cap, m_srcToV[src], numVertices);
        // capacity of srcToV[trg] does not change
        put(m_cap, m_vToTrg[src], 0);
        put(m_cap, m_vToTrg[trg], numVertices);

        // TODO better flow algorithm
        double minCut = boost::boykov_kolmogorov_max_flow(m_auxGraph, m_src, m_trg);
        double violation = numVertices - 1 - minCut;

        if (compare.g(violation, 0)) {
            auto colors = get(boost::vertex_color, m_auxGraph);
            auto srcColor = get(colors, m_src);
            assert(srcColor != get(colors, m_trg));
            m_violatingSet.clear();
            for (auto v : boost::make_iterator_range(vertices(m_auxGraph))) {
                if (v != m_src && v != m_trg && get(colors, v) == srcColor) {
                    m_violatingSet.insert(v);
                }
            }
            m_violatingPair = std::make_pair(src, trg);
        }

        // reset the original values for the capacities
        put(m_cap, m_srcToV[src], origVal);
        // capacity of srcToV[trg] does not change
        put(m_cap, m_vToTrg[src], 1);
        put(m_cap, m_vToTrg[trg], 1);

        return violation;
    }


    AuxGraph m_auxGraph;
    AuxVertex   m_src;
    AuxVertex   m_trg;

    AuxEdgeList  m_srcToV;
    AuxEdgeList  m_vToTrg;

    CandidateList m_candidateList;
    ViolatingSet m_violatingSet;
    Candidate m_violatingPair;

    AuxEdgeCapacity m_cap;
    AuxEdgeReverse  m_rev;
};


} //ir
} //paal
#endif /* BOUNDED_DEGREE_MST_ORACLE_HPP */
