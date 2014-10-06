/**
 * @file budgeted_maximum_coverage_result_check.hpp
 * @brief
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2014-10-09
 */
#ifndef PAAL_BUDGETED_MAXIMUM_COVERAGE_CHECK_HPP
#define PAAL_BUDGETED_MAXIMUM_COVERAGE_CHECK_HPP

#include "test_utils/test_result_check.hpp"

#include <boost/test/unit_test.hpp>

#include <vector>

template <typename Result, typename SetToElements, typename SetToWeight,
          typename SetToCost>
void budgeted_maximum_coverage_result_check(
    Result &result, SetToElements &sets_element, SetToWeight &weight,
    SetToCost &cost_of_set, const int elements, const int optimal,
    const int weight_solution, const double aproximation_ratio, int budget) {
    int weight_verification = 0, cost_verification = 0;
    std::vector<bool> is_covered(elements, false);
    for (auto const &set : result) {
        cost_verification += cost_of_set(set);
        for (auto element : sets_element[set]) {
            if (!is_covered.at(element)) {
                weight_verification += weight(element);
                is_covered[element] = true;
            }
        }
    }
    LOGLN("cost_verification: " << cost_verification);
    BOOST_CHECK(cost_verification <= budget);
    BOOST_CHECK_EQUAL(weight_verification, weight_solution);
    check_result(weight_solution, optimal, aproximation_ratio,
                 paal::utils::greater_equal());
}

#endif /* PAAL_BUDGETED_MAXIMUM_COVERAGE_CHECK_HPP */
