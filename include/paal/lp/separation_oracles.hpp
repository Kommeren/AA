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
 * @class SeparationOracle
 * @brief Separation oracle for the row generation.
 *
 * @tparam ViolationChecker
 */
template <typename ViolationChecker>
class SeparationOracle {
public:
    /// Constructor.
    SeparationOracle(ViolationChecker checker = ViolationChecker())
        : m_violationsChecker(checker)
    { }

    /**
     * Adds a violated constraint to the LP.
     */
    template <typename Problem, typename LP>
    void addViolatedConstraint(const Problem & problem, LP & lp) {
        m_violationsChecker.addViolatedConstraint(m_violatedConstraint, problem, lp);
    }

protected:
    /// violations checker
    ViolationChecker m_violationsChecker;
    /// violated constraint ID
    typename ViolationChecker::Candidate m_violatedConstraint;
};

/**
 * @class MostViolatedSeparationOracle
 * @brief Separation oracle for the row generation, using the most violated strategy.
 *
 * @tparam ViolationChecker
 */
template <typename ViolationChecker>
class MostViolatedSeparationOracle : public SeparationOracle<ViolationChecker> {
    typedef SeparationOracle<ViolationChecker> base;
    using base::m_violationsChecker;
    using base::m_violatedConstraint;
public:
    /// Constructor.
    MostViolatedSeparationOracle(ViolationChecker checker = ViolationChecker())
        : SeparationOracle<ViolationChecker>(checker)
    { }

    /**
     * Checks if the current LP solution is a feasible solution of the problem.
     */
    template <typename Problem, typename LP>
    bool feasibleSolution(const Problem & problem, const LP & lp) {
        double maximumViolation = 0;

        for (auto candidate : m_violationsChecker.getViolationCandidates(problem, lp)) {
            auto violation = m_violationsChecker.checkViolation(candidate, problem);
            if (violation && *violation > maximumViolation) {
                maximumViolation = *violation;
                m_violatedConstraint = candidate;
            }
        }

        return !problem.getCompare().g(maximumViolation, 0);
    }
};

/**
 * @class FirstViolatedSeparationOracle
 * @brief Separation oracle for the row generation, using the first violated strategy.
 *
 * @tparam ViolationChecker
 */
template <typename ViolationChecker>
class FirstViolatedSeparationOracle : public SeparationOracle<ViolationChecker> {
    typedef SeparationOracle<ViolationChecker> base;
    using base::m_violationsChecker;
    using base::m_violatedConstraint;
public:
    /// Constructor.
    FirstViolatedSeparationOracle(ViolationChecker checker = ViolationChecker())
        : SeparationOracle<ViolationChecker>(checker)
    { }

    /**
     * Checks if the current LP solution is a feasible solution of the problem.
     */
    template <typename Problem, typename LP>
    bool feasibleSolution(const Problem & problem, const LP & lp) {
        for (auto candidate : m_violationsChecker.getViolationCandidates(problem, lp)) {
            auto violation = m_violationsChecker.checkViolation(candidate, problem);
            if (violation) {
                m_violatedConstraint = candidate;
                return false;
            }
        }

        return true;
    }
};

/**
 * @class RandomViolatedSeparationOracle
 * @brief Separation oracle for the row generation, using the random violated strategy.
 *
 * @tparam ViolationChecker
 */
template <typename ViolationChecker>
class RandomViolatedSeparationOracle : public SeparationOracle<ViolationChecker> {
    typedef SeparationOracle<ViolationChecker> base;
    using base::m_violationsChecker;
    using base::m_violatedConstraint;
public:
    /// Constructor.
    RandomViolatedSeparationOracle(ViolationChecker checker = ViolationChecker())
        : SeparationOracle<ViolationChecker>(checker)
    { }

    /**
     * Checks if the current LP solution is a feasible solution of the problem.
     */
    template <typename Problem, typename LP>
    bool feasibleSolution(const Problem & problem, const LP & lp) {
        auto const & range = m_violationsChecker.getViolationCandidates(problem, lp);
        auto rangeSize = boost::distance(range);
        if (rangeSize == 0) {
            return true;
        }
        auto middle = std::next(range.begin(), rand() % rangeSize);

        for (auto candidate : boost::join(
                                boost::make_iterator_range(middle, range.end()),
                                boost::make_iterator_range(range.begin(), middle))) {
            auto violation = m_violationsChecker.checkViolation(candidate, problem);
            if (violation) {
                m_violatedConstraint = candidate;
                return false;
            }
        }

        return true;
    }
};

} //lp
} //paal
#endif /* SEPARATION_ORACLES_HPP */
