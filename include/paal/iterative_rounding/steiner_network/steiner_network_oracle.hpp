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
 * @class steiner_network_violation_checker
 * @brief Violations checker for the separation oracle
 *      in the steiner network problem.
 */
class steiner_network_violation_checker {
    using AuxVertex = min_cut_finder::Vertex;
    using Violation = boost::optional<double>;

public:
    using Candidate = std::pair<AuxVertex, AuxVertex>;

    /**
     * Checks if any solution to the problem exists.
     */
    template <typename Problem>
    bool check_if_solution_exists(Problem & problem) {
        const auto & g = problem.get_graph();
        m_min_cut.init(num_vertices(g));

        for (auto e : boost::make_iterator_range(edges(g))) {
            auto u = source(e, g);
            auto v = target(e, g);
            m_min_cut.add_edge_to_graph(u, v, 1, 1);
        }

        for (auto res : problem.get_restrictions_vec()) {
            if (check_violation(res, problem)) {
                return false;
            }
        }

        return true;
    }

    /**
     * Returns an iterator range of violated constraint candidates.
     */
    template <typename Problem, typename LP>
    auto get_violation_candidates(const Problem & problem, const LP & lp) ->
            decltype(problem.get_restrictions_vec()) {

        fill_auxiliary_digraph(problem, lp);
        return problem.get_restrictions_vec();
    }

    /**
     * Checks if the given constraint candidate is violated an if it is,
     * returns the violation value and violated constraint ID.
     */
    template <typename Problem>
    Violation check_violation(Candidate candidate, const Problem & problem) {
        double violation = check_min_cut(candidate.first, candidate.second, problem);
        if (problem.get_compare().g(violation, 0)) {
            return violation;
        }
        else {
            return Violation{};
        }
    }

    /**
     * Adds a violated constraint to the LP.
     */
    template <typename Problem, typename LP>
    void add_violated_constraint(Candidate violation, const Problem & problem, LP & lp) {
        if (violation != m_min_cut.get_last_cut()) {
            check_min_cut(violation.first, violation.second, problem);
        }

        const auto & g = problem.get_graph();
        auto restriction = problem.getMaxRestriction(violation.first, violation.second);

        for (auto const & e : problem.get_edges_in_solution()) {
            if (is_edge_in_violating_cut(e, g)) {
                --restriction;
            }
        }

        lp.add_row(lp::LO, restriction);

        for (auto const & e : problem.get_edge_map()) {
            if (is_edge_in_violating_cut(e.second, g)) {
                lp.add_new_row_coef(e.first);
            }
        }

        lp.load_new_row();
    }

private:

    /**
     * Checks if a given edge belongs to the cut given by the current violating set.
     */
    template <typename Edge, typename Graph>
    bool is_edge_in_violating_cut(Edge edge, const Graph & g) {
        auto u = source(edge, g);
        auto v = target(edge, g);
        return m_min_cut.is_in_source_set(u) != m_min_cut.is_in_source_set(v);
    }

    /**
     * Creates the auxiliary directed graph used for feasibility testing.
     */
    template <typename Problem, typename LP>
    void fill_auxiliary_digraph(Problem & problem, const LP & lp) {
        const auto & g = problem.get_graph();
        m_min_cut.init(num_vertices(g));

        for (auto const & e : problem.get_edge_map()) {
            lp::col_id colIdx = e.first;
            double colVal = lp.get_col_prim(colIdx);

            if (problem.get_compare().g(colVal, 0)) {
                auto u = source(e.second, g);
                auto v = target(e.second, g);
                m_min_cut.add_edge_to_graph(u, v, colVal, colVal);
            }
        }

        for (auto const & e : problem.get_edges_in_solution()) {
            auto u = source(e, g);
            auto v = target(e, g);
            m_min_cut.add_edge_to_graph(u, v, 1, 1);
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
    double check_min_cut(AuxVertex src, AuxVertex trg, const Problem & problem) {
        double minCut = m_min_cut.find_min_cut(src, trg);
        double restriction = problem.getMaxRestriction(src, trg);
        return restriction - minCut;
    }

    min_cut_finder m_min_cut;
};

} //ir
} //paal
#endif /* STEINER_NETWORK_ORACLE_HPP */
