/**
 * @file bounded_degree_mst_oracle_components.hpp
 * @brief 
 * @author Piotr Godlewski
 * @version 1.0
 * @date 2013-06-18
 */
#ifndef BOUNDED_DEGREE_MST_ORACLE_COMPONENTS_HPP
#define BOUNDED_DEGREE_MST_ORACLE_COMPONENTS_HPP

#include "paal/utils/functors.hpp"

namespace paal {
namespace ir {

/**
 * Component of the Bounded Degree MST Separation Oracle.
 * Finds the most violated constraint.
 */
struct FindMostViolated {
    template <typename Problem, typename Oracle>
    bool operator()(Problem & problem, Oracle & oracle, int verticesNum) {
        return oracle.findMostViolatedConstraint(problem);
    };
};

/**
 * Component of the Bounded Degree MST Separation Oracle.
 * Finds the any violated constraint.
 */
struct FindAnyViolated {
    template <typename Problem, typename Oracle>
    bool operator()(Problem & problem, Oracle & oracle, int verticesNum) {
        return oracle.findAnyViolatedConstraint(problem);
    };
};

/**
 * Component of the Bounded Degree MST Separation Oracle.
 * Finds the any violated constraint, starting from a random vertex.
 */
struct FindRandViolated {
    template <typename Problem, typename Oracle>
    bool operator()(Problem & problem, Oracle & oracle, int verticesNum) {
        return oracle.findAnyViolatedConstraint(problem, rand() % verticesNum);
    };
};

/**
 * Component of the Bounded Degree MST Separation Oracle.
 * Runs the initial min-cut test, which may quickly find a violated constraint
 * (but it cannot prove that no constraint is violated).
 */
struct MinCutTest {
    template <typename Oracle>
    bool operator()(Oracle & oracle) {
        return false;
    };
};
  
/**
 * @class BoundedDegreeMSTOracleComponents
 * @brief Components of the separation oracle for the bounded degree minimum spanning tree problem.
 *
 * @tparam FindViolated 
 * @tparam InitialTest 
 */
template <typename FindViolated = FindRandViolated,
          typename InitialTest = utils::ReturnFalseFunctor>
class BoundedDegreeMSTOracleComponents {
public:
    template <typename Problem, typename Oracle>
    bool findViolated(Problem & problem, Oracle & oracle, int verticesNum) {
        return m_findViolated(problem, oracle, verticesNum);
    };
    
    template <typename Oracle>
    bool initialTest(Oracle & oracle) {
        return m_initialTest(oracle);
    };
    
private:  
    FindViolated m_findViolated;
    InitialTest m_initialTest;
};

} //ir
} //paal
#endif /* BOUNDED_DEGREE_MST_ORACLE_COMPONENTS_HPP */
