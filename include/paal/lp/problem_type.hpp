/**
 * @file problem_type.hpp
 * @brief
 * @author Piotr Godlewski
 * @version 1.0
 * @date 2013-10-28
 */
#ifndef PROBLEM_TYPE_HPP
#define PROBLEM_TYPE_HPP

namespace paal {
namespace lp {
/// LP problem type.
enum problem_type {
    OPTIMAL,
    INFEASIBLE,
    UNBOUNDED,
    UNDEFINED
};
}
}
#endif /* PROBLEM_TYPE_HPP */
