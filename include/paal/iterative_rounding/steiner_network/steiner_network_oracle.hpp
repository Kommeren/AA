/**
 * @file steiner_network_oracle.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-06-24
 */
#ifndef STEINER_NETWORK_ORACLE_HPP
#define STEINER_NETWORK_ORACLE_HPP

#include <boost/range/iterator_range.hpp>
#include <boost/range/join.hpp>
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
 * @brief Components of the separation oracle for the steiner network problem.
 */
template <typename... Args>
    using SteinerNetworkOracleComponents =
        data_structures::Components<
        data_structures::NameWithDefault<FindViolated, lp::FindRandViolated> >::type<Args...>;


/**
 * @class SteinerNetworkOracle
 * @brief Separation oracle for the row generation in the steiner network problem.
 *
 * @tparam OracleComponents
 */
template <typename OracleComponents = SteinerNetworkOracleComponents<>>
class SteinerNetworkOracle {
    typedef utils::Compare<double> Compare;
public:
    /**
     * Checks if any solution to the problem exists.
     */
    template <typename Problem>
    bool checkIfSolutionExists(Problem & problem) {
        const auto & g = problem.getGraph();
        m_auxGraph = AuxGraph(num_vertices(g));
        m_cap = get(boost::edge_capacity, m_auxGraph);
        m_rev = get(boost::edge_reverse, m_auxGraph);

        for (auto const & e : boost::make_iterator_range(edges(g))) {
            auto u = source(e, g);
            auto v = target(e, g);
            addEdge(u, v, 1);
        }
        return !findAnyViolatedConstraint(problem);
    }

    /**
     * Checks if the current LP solution is a feasible solution of the problem.
     */
    template <typename Problem, typename LP>
    bool feasibleSolution(Problem & problem, const LP & lp) {
        fillAuxiliaryDigraph(problem, lp);
        return !m_oracleComponents.template call<FindViolated>(
                            problem, *this, problem.getRestrictionsVec().size());
    }

    /**
     * Adds a violated constraint to the LP.
     */
    template <typename Problem, typename LP>
    void addViolatedConstraint(Problem & problem, LP & lp) {
        const auto & g = problem.getGraph();
        auto restriction = problem.getMaxRestriction(
                                m_violatedRestriction.first,
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

    /**
     * Finds any violated constraint and saves it or decides that no constraint is violated.
     *
     * @param problem Problem object
     * @param startIndex index of the restriction from which we begin the search
     * @return true iff a violated constraint was found
     */
    template <typename Problem>
    bool findAnyViolatedConstraint(Problem & problem, int startIndex = 0) {
        const auto & restrictionsVec = problem.getRestrictionsVec();
        auto startIter = restrictionsVec.begin();
        std::advance(startIter, startIndex);

        for (auto const & src_trg : boost::join(boost::make_iterator_range(startIter, restrictionsVec.end()),
                                        boost::make_iterator_range(restrictionsVec.begin(), startIter))) {
            assert(src_trg.first != src_trg.second);
            if (problem.getCompare().g(checkViolationBiggerThan(problem, src_trg.first, src_trg.second), 0)) {
                return true;
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
        double max = 0;

        for (auto const & src_trg : problem.getRestrictionsVec()) {
            assert(src_trg.first != src_trg.second);
            max = std::max(checkViolationBiggerThan(problem, src_trg.first, src_trg.second, max), max);
        }

        return problem.getCompare().g(max, 0);
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
    typedef std::unordered_set<AuxVertex> ViolatingSet;

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
     * \c trg and saves it if the violation is greater than \c minViolation
     * @param src vertex to be contained in the violating set
     * @param trg vertex not to be contained in the violating set
     * @param minViolation minimum violation that a set should have to be saved
     * @return violation of the found set
     */
    template <typename Problem>
    double checkViolationBiggerThan(Problem & problem, AuxVertex src, AuxVertex trg,
                double minViolation = 0.) {
        double minCut = boost::boykov_kolmogorov_max_flow(m_auxGraph, src, trg);
        double restriction = problem.getMaxRestriction(src, trg);
        double violation = restriction - minCut;

        if (problem.getCompare().g(violation, minViolation)) {
            m_violatedRestriction = std::make_pair(src, trg);
            m_violatingSet.clear();

            auto colors = get(boost::vertex_color, m_auxGraph);
            auto srcColor = get(colors, src);
            assert(srcColor != get(colors, trg));
            for (auto v : boost::make_iterator_range(vertices(m_auxGraph))) {
                if (v != trg && get(colors, v) == srcColor) {
                    m_violatingSet.insert(v);
                }
            }
        }

        return violation;
    }

    OracleComponents m_oracleComponents;

    AuxGraph m_auxGraph;

    ViolatingSet m_violatingSet;
    std::pair<AuxVertex, AuxVertex> m_violatedRestriction;

    AuxEdgeCapacity m_cap;
    AuxEdgeReverse  m_rev;
};

} //ir
} //paal
#endif /* STEINER_NETWORK_ORACLE_HPP */
