/**
 * @file steiner_tree_oracle.hpp
 * @brief
 * @author Maciej Andrejczuk, Piotr Godlewski
 * @version 1.0
 * @date 2013-08-01
 */
#ifndef STEINER_TREE_ORACLE_HPP
#define STEINER_TREE_ORACLE_HPP

#include "paal/iterative_rounding/min_cut.hpp"

#include <unordered_map>
#include <vector>
#include <boost/optional.hpp>

namespace paal {
namespace ir {

/**
 * @class SteinerTreeViolationChecker
 * @brief Violations checker for the separation oracle
 *      in the steiner tree problem.
 */
class SteinerTreeViolationChecker {
    typedef MinCutFinder::Edge AuxEdge;
    typedef MinCutFinder::Vertex AuxVertex;
    typedef std::vector<AuxEdge> AuxEdgeList;
    typedef boost::optional<double> Violation;

public:
    typedef AuxVertex Candidate;

    SteinerTreeViolationChecker() :
        m_currentGraphSize(-1) {}

    /**
     * Returns an iterator range of violated constraint candidates.
     */
    template <typename Problem, typename LP>
    auto getViolationCandidates(const Problem & problem, const LP & lp)
            -> decltype(problem.getTerminals()) {

        int graphSize = problem.getTerminals().size();
        if (graphSize != m_currentGraphSize) {
            // Graph has changed, construct new oracle
            m_currentGraphSize = graphSize;
            m_root = selectRoot(problem.getTerminals());
            createAuxiliaryDigraph(problem, lp);
        } else {
            updateAuxiliaryDigraph(problem, lp);
        }
        return problem.getTerminals();
    }

    /**
     * Checks if the given constraint candidate is violated an if it is,
     * returns the violation value and violated constraint ID.
     */
    template <typename Problem>
    Violation checkViolation(Candidate candidate, const Problem & problem) {
        if (candidate == m_root) {
            return Violation();
        }

        double violation = checkMinCut(candidate);
        if (problem.getCompare().g(violation, 0)) {
            return Violation(violation);
        }
        else {
            return Violation();
        }
    }

    /**
     * Adds the violated constraint to LP.
     * It contains all the components reachable from a given source,
     * but its sink vertex is not reachable.
     */
    template <typename Problem, typename LP>
    void addViolatedConstraint(Candidate violatingTerminal, const Problem & problem, LP & lp) {
        if (std::make_pair(violatingTerminal, m_root) != m_minCut.getLastCut()) {
            checkMinCut(violatingTerminal);
        }

        const auto & components = problem.getComponents();
        lp.addRow(lp::LO, 1);
        for (int i = 0; i < components.size(); ++i) {
            auto u = m_artifVertices[i];
            int ver = components.findVersion(i);
            auto v = components.find(i).getSink(ver);
            if (m_minCut.isInSourceSet(u) && !m_minCut.isInSourceSet(v)) {
                lp::ColId colIdx = problem.findColumnLP(i);
                lp.addNewRowCoef(colIdx);
            }
        }
        lp.loadNewRow();
    }

private:

    /**
     * @brief Creates the auxiliary directed graph used for feasibility testing
     *
     * Graph contains a vertex for each component and each terminal
     * Sources of every component have out edges with infinite weight
     * Target has in edge with weigth x_i from LP
     */
    template <typename Problem, typename LP>
    void createAuxiliaryDigraph(Problem &problem, const LP & lp) {
        m_minCut.init(problem.getTerminals().size());
        m_artifVertices.clear();
        const auto & components = problem.getComponents();

        for (int i = 0; i < components.size(); ++i) {
            AuxVertex newV = m_minCut.addVertex();
            m_artifVertices[i] = newV;
            int ver = components.findVersion(i);
            auto sink = components.find(i).getSink(ver);
            for (auto w : boost::make_iterator_range(components.find(i).getElements())) {
                if (w != sink) {
                    double INF = std::numeric_limits<double>::max();
                    m_minCut.addEdge(w, newV, INF);
                } else {
                    lp::ColId x = problem.findColumnLP(i);
                    double colVal = lp.getColPrim(x);
                    m_minCut.addEdge(newV, sink, colVal);
                }
            }
        }
    }

    /**
     * Updates the auxiliary directed graph. Should be performed after each LP iteration.
     */
    template <typename Problem, typename LP>
    void updateAuxiliaryDigraph(Problem &problem, const LP & lp) {
        const auto & components = problem.getComponents();
        for (int i = 0; i < components.size(); ++i) {
            AuxVertex componentV = m_artifVertices[i];
            int ver = components.findVersion(i);
            auto sink = components.find(i).getSink(ver);
            lp::ColId x = problem.findColumnLP(i);
            double colVal = lp.getColPrim(x);
            m_minCut.addEdge(componentV, sink, colVal);
        }
    }

    /**
     * Select the root terminal. Max-flow will be directed to that vertex during LP oracle execution.
     */
    template <typename Terminals>
    AuxVertex selectRoot(const Terminals & terminals) {
        //TODO: Maybe it's better to select random vertex rather than first
        AuxVertex ret = *terminals.begin();
        return ret;
    }

    /**
     * Runs a maxflow algorithm between given source and root.
     */
    double checkMinCut(AuxVertex src) {
        double minCut = m_minCut.findMinCut(src, m_root);
        return 1 - minCut;
    }

    AuxVertex m_root; // root vertex, sink of all max-flows
    int m_currentGraphSize; // size of current graph

    std::unordered_map<int, AuxVertex> m_artifVertices; // maps componentId to auxGraph vertex

    MinCutFinder m_minCut;
};

} //ir
} //paal
#endif /* STEINER_TREE_ORACLE_HPP */
