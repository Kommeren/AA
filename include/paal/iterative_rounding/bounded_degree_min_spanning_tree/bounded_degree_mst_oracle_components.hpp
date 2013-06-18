/**
 * @file bounded_degree_mst_oracle_components.hpp
 * @brief 
 * @author Piotr Godlewski
 * @version 1.0
 * @date 2013-06-18
 */
#ifndef BOUNDED_DEGREE_MST_ORACLE_COMPONENTS_HPP
#define BOUNDED_DEGREE_MST_ORACLE_COMPONENTS_HPP

#include "paal/utils/do_nothing_functor.hpp"

namespace paal {
namespace ir {

struct FindMostViolated {
    template <typename Oracle>
    bool operator()(Oracle & oracle, int verticesNum) {
        return oracle.findMostViolatedConstraint();
    };
};

struct FindAnyViolated {
    template <typename Oracle>
    bool operator()(Oracle & oracle, int verticesNum) {
        return oracle.findAnyViolatedConstraint(1);
    };
};

struct FindRandViolated {
    template <typename Oracle>
    bool operator()(Oracle & oracle, int verticesNum) {
        return oracle.findAnyViolatedConstraint(rand() % verticesNum + 1);
    };
};

// TODO implement min cut test
struct MinCutTest {
    template <typename Oracle>
    bool operator()(Oracle & oracle) {
        return false;
    };
};
  
/**
 * @class BoundedDegreeMSTOracleComponents
 * @brief components of the separation oracle for the bounded degree minimum spanning tree problem.
 *
 * @tparam FindViolated 
 * @tparam InitialTest 
 */
template <typename FindViolated = FindRandViolated,
          typename InitialTest = utils::ReturnFalseFunctor>
class BoundedDegreeMSTOracleComponents {
public:
    template <typename Oracle>
    bool findViolated(Oracle & oracle, int verticesNum) {
        return m_findViolated(oracle, verticesNum);
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
