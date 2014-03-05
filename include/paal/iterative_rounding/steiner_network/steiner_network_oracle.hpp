/**
 * @file steiner_network_oracle.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-06-24
 */
#ifndef STEINER_NETWORK_ORACLE_HPP
#define STEINER_NETWORK_ORACLE_HPP

#include "paal/iterative_rounding/min_cut.hpp"


namespace paal {
namespace ir {

/**
 * @class SteinerNetworkViolationChecker
 * @brief Violations checker for the separation oracle
 *      in the steiner network problem.
 */
class SteinerNetworkViolationChecker {
    typedef MinCutFinder::Vertex AuxVertex;
    typedef boost::optional<double> Violation;

public:
    typedef std::pair<AuxVertex, AuxVertex> Candidate;

    /**
     * Checks if any solution to the problem exists.
     */
    template <typename Problem>
    bool checkIfSolutionExists(Problem & problem) {
        const auto & g = problem.getGraph();
        m_minCut.init(num_vertices(g));

        for (auto e : boost::make_iterator_range(edges(g))) {
            auto u = source(e, g);
            auto v = target(e, g);
            m_minCut.addEdge(u, v, 1, 1);
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
        if (violation != m_minCut.getLastCut()) {
            checkMinCut(violation.first, violation.second, problem);
        }

        const auto & g = problem.getGraph();
        auto restriction = problem.getMaxRestriction(violation.first, violation.second);

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
        return m_minCut.isInSourceSet(u) != m_minCut.isInSourceSet(v);
    }

    /**
     * Creates the auxiliary directed graph used for feasibility testing.
     */
    template <typename Problem, typename LP>
    void fillAuxiliaryDigraph(Problem & problem, const LP & lp) {
        const auto & g = problem.getGraph();
        m_minCut.init(num_vertices(g));

        for (auto const & e : problem.getEdgeMap()) {
            lp::ColId colIdx = e.first;
            double colVal = lp.getColPrim(colIdx);

            if (problem.getCompare().g(colVal, 0)) {
                auto u = source(e.second, g);
                auto v = target(e.second, g);
                m_minCut.addEdge(u, v, colVal, colVal);
            }
        }

        for (auto const & e : problem.getEdgesInSolution()) {
            auto u = source(e, g);
            auto v = target(e, g);
            m_minCut.addEdge(u, v, 1, 1);
        }
    }

    /**
     * Finds the most violated set of vertices containing \c src and not containing \c trg.
     * @param src vertex to be contained in the violating set
     * @param trg vertex not to be contained in the violating set
     * @param problem problem object
     * @return violation of the found set
     */
    template <typename Problem>
    double checkMinCut(AuxVertex src, AuxVertex trg, const Problem & problem) {
        double minCut = m_minCut.findMinCut(src, trg);
        double restriction = problem.getMaxRestriction(src, trg);
        return restriction - minCut;
    }

    MinCutFinder m_minCut;
};

} //ir
} //paal
#endif /* STEINER_NETWORK_ORACLE_HPP */
