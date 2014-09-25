//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file separation_oracles.hpp
 * @brief
 * @author Piotr Godlewski
 * @version 1.0
 * @date 2014-02-24
 */
#ifndef SEPARATION_ORACLES_HPP
#define SEPARATION_ORACLES_HPP

#include <boost/range/distance.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/range/join.hpp>

namespace paal {
namespace lp {

/**
 * @class separation_oracle
 * @brief Separation oracle for the row generation.
 *
 * @tparam ViolationChecker
 */
template <typename ViolationChecker> class separation_oracle {
  public:
    /// Constructor.
    separation_oracle(ViolationChecker checker = ViolationChecker())
        : m_violations_checker(checker) { }

    /**
     * Adds a violated constraint to the LP.
     */
    template <typename Problem, typename LP>
    void add_violated_constraint(const Problem &problem, LP &lp) {
        m_violations_checker.add_violated_constraint(m_violated_constraint,
                                                     problem, lp);
    }

  protected:
    /// violations checker
    ViolationChecker m_violations_checker;
    /// violated constraint ID
    typename ViolationChecker::Candidate m_violated_constraint;
};

/**
 * @class most_violated_separation_oracle
 * @brief Separation oracle for the row generation,
 *   using the most violated strategy.
 * @tparam ViolationChecker
 */
template <typename ViolationChecker>
class most_violated_separation_oracle : public separation_oracle<ViolationChecker> {
    using base = separation_oracle<ViolationChecker>;
    using base::m_violations_checker;
    using base::m_violated_constraint;

  public:
    /// Constructor.
    most_violated_separation_oracle(ViolationChecker checker = ViolationChecker())
        : separation_oracle<ViolationChecker>(checker) { }

    /**
     * Checks if the current LP solution is a feasible solution of the problem.
     */
    template <typename Problem, typename LP>
    bool feasible_solution(const Problem & problem, const LP & lp) {
        double maximum_violation = 0;
        for (auto candidate : m_violations_checker.get_violation_candidates(problem, lp)) {
            auto violation = m_violations_checker.check_violation(candidate, problem);
            if (violation && *violation > maximum_violation) {
                maximum_violation = *violation;
                m_violated_constraint = candidate;
            }
        }
        return !problem.get_compare().g(maximum_violation, 0);
    }
};

/**
 * @class first_violated_separation_oracle
 * @brief Separation oracle for the row generation,
 *   using the first violated strategy.
 * @tparam ViolationChecker
 */
template <typename ViolationChecker>
class first_violated_separation_oracle : public separation_oracle<ViolationChecker> {
    using base = separation_oracle<ViolationChecker>;
    using base::m_violations_checker;
    using base::m_violated_constraint;

  public:
    /// Constructor.
    first_violated_separation_oracle(ViolationChecker checker = ViolationChecker())
        : separation_oracle<ViolationChecker>(checker) { }

    /**
     * Checks if the current LP solution is a feasible solution of the problem.
     */
    template <typename Problem, typename LP>
    bool feasible_solution(const Problem &problem, const LP &lp) {
        for (auto candidate :
             m_violations_checker.get_violation_candidates(problem, lp)) {
            auto violation =
                m_violations_checker.check_violation(candidate, problem);
            if (violation) {
                m_violated_constraint = candidate;
                return false;
            }
        }
        return true;
    }
};

/**
 * @class random_violated_separation_oracle
 * @brief Separation oracle for the row generation,
 *   using the random violated strategy.
 * @tparam ViolationChecker
 */
template <typename ViolationChecker>
class random_violated_separation_oracle : public separation_oracle<ViolationChecker> {
    using base = separation_oracle<ViolationChecker>;
    using base::m_violations_checker;
    using base::m_violated_constraint;

  public:
    /// Constructor.
    random_violated_separation_oracle(ViolationChecker checker = ViolationChecker())
        : separation_oracle<ViolationChecker>(checker) { }

    /**
     * Checks if the current LP solution is a feasible solution of the problem.
     */
    template <typename Problem, typename LP>
    bool feasible_solution(const Problem & problem, const LP & lp) {
        auto const & range = m_violations_checker.get_violation_candidates(problem, lp);
        auto range_size = boost::distance(range);
        if (range_size == 0) {
            return true;
        }
        auto middle = std::next(range.begin(), rand() % range_size);
        for (auto candidate : boost::join(
                        boost::make_iterator_range(middle, range.end()),
                        boost::make_iterator_range(range.begin(), middle))) {
            auto violation = m_violations_checker.check_violation(candidate, problem);
            if (violation) {
                m_violated_constraint = candidate;
                return false;
            }
        }
        return true;
    }
};

} //! lp
} //! paal
#endif /* SEPARATION_ORACLES_HPP */
