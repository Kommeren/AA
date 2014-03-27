/**
 * @file lp_row_generation.hpp
 * @brief
 * @author Piotr Godlewski
 * @version 1.0
 * @date 2013-06-04
 */
#ifndef LP_ROW_GENERATION_HPP
#define LP_ROW_GENERATION_HPP

#include "paal/lp/problem_type.hpp"

namespace paal {
namespace lp {

/**
 * @class lp_row_generation
 *
 * @brief Finds an extreme point solution to the LP using the row generation technique.
 */
class lp_row_generation {
public:
    /**
     * Finds an extreme point solution to the LP using the row genereation technique:
     * solves the initial LP and then ask the separation oracle if the found solution
     * is a feasible solution to the complete problem. If not, adds a new row (generated by
     * the oracle) to the LP and reoptimizes it. This procedure is iterated until a
     * feasible solution to the full LP is found.
     */
    template <typename Problem, typename LP>
    problem_type operator()(Problem & problem, LP & lp, problem_type probType) {
        auto & oracle = problem.get_oracle();
        while (probType == OPTIMAL && !oracle.feasible_solution(problem, lp)) {
            oracle.add_violated_constraint(problem, lp);
            probType = lp.resolve_to_extreme_point_dual();
        }
        return probType;
    }
};

/**
 * @class row_generation_solve_lp
 *
 * @brief Finds an extreme point solution to the LP using the row generation technique.
 */
class row_generation_solve_lp {
public:
    /**
     * Finds an extreme point solution to the LP using the row genereation technique.
     */
    template <typename Problem, typename LP>
    problem_type operator()(Problem & problem, LP & lp) {
        return m_row_generation(problem, lp, lp.solve_to_extreme_point_primal());
    }

private:
    lp_row_generation m_row_generation;
};

/**
 * @class row_generation_resolve_lp
 *
 * @brief Finds an extreme point solution to the LP using the row generation technique.
 */
class row_generation_resolve_lp {
public:
    /**
     * Finds an extreme point solution to the LP using the row genereation technique.
     */
    template <typename Problem, typename LP>
    problem_type operator()(Problem & problem, LP & lp) {
        return m_row_generation(problem, lp, lp.resolve_to_extreme_point_primal());
    }

private:
    lp_row_generation m_row_generation;
};

} //lp
} //paal

#endif /* LP_ROW_GENERATION_HPP */
