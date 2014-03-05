/**
 * @file bounded_degree_mst_oracle.hpp
 * @brief
 * @author Piotr Godlewski
 * @version 1.0
 * @date 2013-06-05
 */
#ifndef BOUNDED_DEGREE_MST_ORACLE_HPP
#define BOUNDED_DEGREE_MST_ORACLE_HPP

#include "paal/iterative_rounding/min_cut.hpp"

#include <boost/optional.hpp>
#include <vector>


namespace paal {
namespace ir {

/**
 * @class BDMSTViolationChecker
 * @brief Violations checker for the separation oracle
 *      in the bounded degree minimum spanning tree problem.
 */
class BDMSTViolationChecker {
    typedef MinCutFinder::Edge AuxEdge;
    typedef MinCutFinder::Vertex AuxVertex;
    typedef std::vector<AuxEdge> AuxEdgeList;
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
        initializeCandidates(problem);
        return m_candidateList;
    }

    /**
     * Checks if the given constraint candidate is violated an if it is,
     * returns the violation value and violated constraint ID.
     */
    template <typename Problem>
    Violation checkViolation(Candidate candidate, const Problem & problem) {
        double violation = checkMinCut(candidate.first, candidate.second);
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
        if (violatingPair != m_minCut.getLastCut()) {
            checkMinCut(violatingPair.first, violatingPair.second);
        }

        const auto & g = problem.getGraph();
        lp.addRow(lp::UP, 0, m_minCut.sourceSetSize() - 2);

        for (auto const & e : problem.getEdgeMap().right) {
            auto u = source(e.second, g);
            auto v = target(e.second, g);
            if (m_minCut.isInSourceSet(u) && m_minCut.isInSourceSet(v)) {
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
        m_verticesNum = num_vertices(g);
        m_minCut.init(m_verticesNum);
        m_srcToV.resize(m_verticesNum);
        m_vToTrg.resize(m_verticesNum);

        for (auto const & e : problem.getEdgeMap().right) {
            lp::ColId colIdx = e.first;
            double colVal = lp.getColPrim(colIdx) / 2;

            if (!problem.getCompare().e(colVal, 0)) {
                auto u = source(e.second, g);
                auto v = target(e.second, g);
                m_minCut.addEdge(u, v, colVal, colVal);
            }
        }

        m_src = m_minCut.addVertex();
        m_trg = m_minCut.addVertex();

        for (auto v : boost::make_iterator_range(vertices(g))) {
            m_srcToV[v] = m_minCut.addEdge(m_src, v, degreeOf(problem, v, lp) / 2).first;
            m_vToTrg[v] = m_minCut.addEdge(v, m_trg, 1).first;
        }
    }

    /**
     * Initializes the list of cut candidates.
     */
    template <typename Problem>
    void initializeCandidates(const Problem & problem) {
        const auto & g = problem.getGraph();
        auto src = *(std::next(vertices(g).first, rand() % m_verticesNum));
        m_candidateList.clear();
        for (auto v : boost::make_iterator_range(vertices(g))) {
            if (v != src) {
                m_candidateList.push_back(std::make_pair(src, v));
                m_candidateList.push_back(std::make_pair(v, src));
            }
        }
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
     * Finds the most violated set of vertices containing \c src and not containing \c trg.
     * @param src vertex to be contained in the violating set
     * @param trg vertex not to be contained in the violating set
     * @return violation of the found set
     */
    double checkMinCut(AuxVertex src, AuxVertex trg) {
        double origCap = m_minCut.getCapacity(m_srcToV[src]);

        m_minCut.setCapacity(m_srcToV[src], m_verticesNum);
        // capacity of srcToV[trg] does not change
        m_minCut.setCapacity(m_vToTrg[src], 0);
        m_minCut.setCapacity(m_vToTrg[trg], m_verticesNum);

        double minCut = m_minCut.findMinCut(m_src, m_trg);
        double violation = m_verticesNum - 1 - minCut;

        // reset the original values for the capacities
        m_minCut.setCapacity(m_srcToV[src], origCap);
        // capacity of srcToV[trg] does not change
        m_minCut.setCapacity(m_vToTrg[src], 1);
        m_minCut.setCapacity(m_vToTrg[trg], 1);

        return violation;
    }

    int m_verticesNum;

    AuxVertex   m_src;
    AuxVertex   m_trg;

    AuxEdgeList  m_srcToV;
    AuxEdgeList  m_vToTrg;

    CandidateList m_candidateList;

    MinCutFinder m_minCut;
};


} //ir
} //paal
#endif /* BOUNDED_DEGREE_MST_ORACLE_HPP */
