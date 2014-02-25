/**
 * @file steiner_network_oracle.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-06-24
 */
#ifndef STEINER_NETWORK_ORACLE_HPP
#define STEINER_NETWORK_ORACLE_HPP

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/boykov_kolmogorov_max_flow.hpp>

#include <unordered_set>


namespace paal {
namespace ir {


/**
 * @class SteinerNetworkViolationChecker
 * @brief Violations checker for the separation oracle
 *      in the steiner network problem.
 */
class SteinerNetworkViolationChecker {
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
    typedef std::unordered_set<AuxVertex> ViolatingSet;
    typedef boost::optional<double> Violation;

public:
    typedef std::pair<AuxVertex, AuxVertex> Candidate;

    /**
     * Checks if any solution to the problem exists.
     */
    template <typename Problem>
    bool checkIfSolutionExists(Problem & problem) {
        const auto & g = problem.getGraph();
        m_auxGraph = AuxGraph(num_vertices(g));
        m_cap = get(boost::edge_capacity, m_auxGraph);
        m_rev = get(boost::edge_reverse, m_auxGraph);

        for (auto e : boost::make_iterator_range(edges(g))) {
            auto u = source(e, g);
            auto v = target(e, g);
            addEdge(u, v, 1);
        }

        for (auto res : problem.getRestrictionsVec()) {
            if (checkViolation(res, problem)) {
                return false;
            }
        }

        return true;
    }

    /**
     * Returns an iterator range of violated constraint candidates.
     */
    template <typename Problem, typename LP>
    auto getViolationCandidates(const Problem & problem, const LP & lp) ->
            decltype(problem.getRestrictionsVec()) {

        fillAuxiliaryDigraph(problem, lp);
        return problem.getRestrictionsVec();
    }

    /**
     * Checks if the given constraint candidate is violated an if it is,
     * returns the violation value and violated constraint ID.
     */
    template <typename Problem>
    Violation checkViolation(Candidate candidate, const Problem & problem) {
        double violation = checkMinCut(candidate.first, candidate.second, problem);
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
    void addViolatedConstraint(Candidate violation, const Problem & problem, LP & lp) {
        if (violation != m_violatedRestriction) {
            checkMinCut(violation.first, violation.second, problem);
        }

        const auto & g = problem.getGraph();
        auto restriction = problem.getMaxRestriction(m_violatedRestriction.first,
                                                        m_violatedRestriction.second);

        for (auto const & e : problem.getEdgesInSolution()) {
            if (isEdgeInViolatingCut(e, g)) {
                --restriction;
            }
        }

        lp.addRow(lp::LO, restriction);

        for (auto const & e : problem.getEdgeMap()) {
            if (isEdgeInViolatingCut(e.second, g)) {
                lp.addNewRowCoef(e.first);
            }
        }

        lp.loadNewRow();
    }

private:

    /**
     * Checks if a given edge belongs to the cut given by the current violating set.
     */
    template <typename Edge, typename Graph>
    bool isEdgeInViolatingCut(Edge edge, const Graph & g) {
        auto u = source(edge, g);
        auto v = target(edge, g);

        return (m_violatingSet.find(u) != m_violatingSet.end()) !=
                    (m_violatingSet.find(v) != m_violatingSet.end());
    }

    /**
     * Creates the auxiliary directed graph used for feasibility testing.
     */
    template <typename Problem, typename LP>
    void fillAuxiliaryDigraph(Problem & problem, const LP & lp) {
        const auto & g = problem.getGraph();
        m_auxGraph = AuxGraph(num_vertices(g));
        m_cap = get(boost::edge_capacity, m_auxGraph);
        m_rev = get(boost::edge_reverse, m_auxGraph);

        for (auto const & e : problem.getEdgeMap()) {
            lp::ColId colIdx = e.first;
            double colVal = lp.getColPrim(colIdx);

            if (problem.getCompare().g(colVal, 0)) {
                auto u = source(e.second, g);
                auto v = target(e.second, g);
                addEdge(u, v, colVal);
            }
        }

        for (auto const & e : problem.getEdgesInSolution()) {
            auto u = source(e, g);
            auto v = target(e, g);
            addEdge(u, v, 1);
        }
    }

    /**
     * Adds an edge to the auxiliary graph.
     * @param vSrc source vertex of for the added edge
     * @param vTrg target vertex of for the added edge
     * @param cap capacity of the added edge
     * @return created edge of the auxiliary graph
     *
     * @tparam SrcVertex
     * @tparam TrgVertex
     */
    template <typename SrcVertex, typename TrgVertex>
    AuxEdge addEdge(const SrcVertex & vSrc, const TrgVertex & vTrg, double cap) {
        bool b, bRev;
        AuxEdge e, eRev;

        std::tie(e, b) = add_edge(vSrc, vTrg, m_auxGraph);
        std::tie(eRev, bRev) = add_edge(vTrg, vSrc, m_auxGraph);

        assert(b && bRev);

        m_cap[e] = cap;
        m_cap[eRev] = cap;

        m_rev[e] = eRev;
        m_rev[eRev] = e;

        return e;
    }

    /**
     * Finds the most violated set of vertices containing \c src and not containing
     * \c trg and saves it if a violation is found.
     * @param src vertex to be contained in the violating set
     * @param trg vertex not to be contained in the violating set
     * @param problem problem object
     * @return violation of the found set
     */
    template <typename Problem>
    double checkMinCut(AuxVertex src, AuxVertex trg, const Problem & problem) {
        assert(src != trg);
        double minCut = boost::boykov_kolmogorov_max_flow(m_auxGraph, src, trg);
        double restriction = problem.getMaxRestriction(src, trg);
        double violation = restriction - minCut;

        if (problem.getCompare().g(violation, 0)) {
            auto colors = get(boost::vertex_color, m_auxGraph);
            auto srcColor = get(colors, src);
            assert(srcColor != get(colors, trg));
            m_violatingSet.clear();
            for (auto v : boost::make_iterator_range(vertices(m_auxGraph))) {
                if (v != trg && get(colors, v) == srcColor) {
                    m_violatingSet.insert(v);
                }
            }
            m_violatedRestriction = std::make_pair(src, trg);
        }

        return violation;
    }

    AuxGraph m_auxGraph;

    ViolatingSet m_violatingSet;
    Candidate m_violatedRestriction;

    AuxEdgeCapacity m_cap;
    AuxEdgeReverse  m_rev;
};

} //ir
} //paal
#endif /* STEINER_NETWORK_ORACLE_HPP */
