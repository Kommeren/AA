/**
 * @file separation_oracle_components.hpp
 * @brief
 * @author Piotr Godlewski
 * @version 1.0
 * @date 2014-01-31
 */
#ifndef SEPARATION_ORACLE_COMPONENTS_HPP
#define SEPARATION_ORACLE_COMPONENTS_HPP


namespace paal {
namespace lp {

/**
 * Component of a separation oracle.
 * Finds the most violated constraint.
 */
struct FindMostViolated {
    template <typename Problem, typename Oracle>
    bool operator()(Problem & problem, Oracle & oracle, int) {
        return oracle.findMostViolatedConstraint(problem);
    };
};

/**
 * Component of a separation oracle.
 * Finds the any violated constraint.
 */
struct FindAnyViolated {
    template <typename Problem, typename Oracle>
    bool operator()(Problem & problem, Oracle & oracle, int) {
        return oracle.findAnyViolatedConstraint(problem);
    };
};

/**
 * Component of a separation oracle.
 * Finds the any violated constraint, starting the search from a random constraint.
 */
struct FindRandViolated {
    template <typename Problem, typename Oracle>
    bool operator()(Problem & problem, Oracle & oracle, int numOfStartingPositions) {
        return oracle.findAnyViolatedConstraint(problem, rand() % numOfStartingPositions);
    };
};

} //lp
} //paal

#endif /* SEPARATION_ORACLE_COMPONENTS_HPP */
