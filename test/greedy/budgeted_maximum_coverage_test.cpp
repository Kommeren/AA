/**
 * @file budgeted_maximum_coverage_test.cpp
 * @brief
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2014-02-17
 */

#include "test_utils/logger.hpp"
#include "test_utils/test_result_check.hpp"
#include "test_utils/budgeted_maximum_coverage_check.hpp"

#include "paal/greedy/set_cover/budgeted_maximum_coverage.hpp"
#include "paal/utils/functors.hpp"
#include "paal/utils/irange.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/range/irange.hpp>

#include <vector>
#include <iterator>
#include <cmath>

using paal::utils::make_array_to_functor;
using paal::utils::identity_functor;
using paal::utils::return_one_functor;

BOOST_AUTO_TEST_CASE(budgeted_maximum_coverage_3_set_optimal) {
    LOGLN("budgeted_maximum_coverage_3_set_optimal");
    const int OPTIMAL = 111;
    const int BUDGET = 111;
    const int ELEMENTS = 3;
    const double APPROXIMATION_RATIO = 1 - 1 / std::exp(1.0);
    std::vector<std::vector<int>> sets_element = { { 0 }, { 1 }, { 2 } };
    std::vector<int> weight = { 100, 10, 1 }, cost_of_set = { 100, 10, 1 };
    auto sets = paal::irange(3);

    std::vector<int> result;
    auto element_index = identity_functor{};
    auto set_to_cost = make_array_to_functor(cost_of_set);
    auto element_to_weight = make_array_to_functor(weight);
    auto covered_weight = paal::greedy::budgeted_maximum_coverage(
        sets, set_to_cost, make_array_to_functor(sets_element),
        std::back_inserter(result), element_index, BUDGET, element_to_weight,
        3);
    budgeted_maximum_coverage_result_check(
        result, sets_element, element_to_weight, set_to_cost, ELEMENTS, OPTIMAL,
        covered_weight, APPROXIMATION_RATIO, BUDGET);
    BOOST_CHECK_EQUAL(covered_weight, OPTIMAL);
}

BOOST_AUTO_TEST_CASE(budgeted_maximum_coverage_empty_set) {
    LOGLN("budgeted_maximum_coverage_empty_set");
    const int OPTIMAL = 0;
    const int BUDGET = 1000;
    const int ELEMENTS = 0;
    const double APPROXIMATION_RATIO = 1 - 1 / std::exp(1.0);
    std::vector<std::vector<int>> sets_element(1);
    std::vector<int> weight, cost_of_set{1000};
    auto sets = paal::irange(1);

    std::vector<int> result;
    auto element_index = identity_functor{};
    auto set_to_cost = make_array_to_functor(cost_of_set);
    auto element_to_weight = make_array_to_functor(weight);
    auto covered_weight = paal::greedy::budgeted_maximum_coverage(
        sets, set_to_cost,
        make_array_to_functor(sets_element),
        std::back_inserter(result), element_index, BUDGET,
        element_to_weight);
    budgeted_maximum_coverage_result_check(
        result, sets_element, element_to_weight, set_to_cost, ELEMENTS, OPTIMAL,
        covered_weight, APPROXIMATION_RATIO, BUDGET);
}

BOOST_AUTO_TEST_CASE(budgeted_maximum_coverage) {
    LOGLN("budgeted_maximum_coverage");
    const int OPTIMAL = 200;
    const int BUDGET = 1000;
    const int ELEMENTS = 21;
    const double APPROXIMATION_RATIO = 1 - 1 / std::exp(1.0);
    std::vector<std::vector<int>> sets_element(21);
    std::vector<int> weight, cost_of_set;
    auto sets = paal::irange(ELEMENTS);
    for (auto i : sets) {
        sets_element[i].push_back(i);
        if (i < 10) {
            weight.push_back(10);
            cost_of_set.push_back(1);
        } else if (i != 20) {
            weight.push_back(1);
            cost_of_set.push_back(1);
        } else {
            weight.push_back(100);
            cost_of_set.push_back(990);
        }
    }
    std::vector<int> result;
    auto element_index = identity_functor{};
    auto set_to_cost = make_array_to_functor(cost_of_set);
    auto element_to_weight = make_array_to_functor(weight);
    auto covered_weight = paal::greedy::budgeted_maximum_coverage(
        sets, set_to_cost,
        make_array_to_functor(sets_element),
        std::back_inserter(result), element_index, BUDGET,
        element_to_weight);
    budgeted_maximum_coverage_result_check(
        result, sets_element, element_to_weight, set_to_cost, ELEMENTS, OPTIMAL,
        covered_weight, APPROXIMATION_RATIO, BUDGET);
}

BOOST_AUTO_TEST_CASE(BudgetMaximumCoverage2) {
    LOGLN("BudgetMaximumCoverage2");
    static const int OPTIMAL = 14;
    static const int ELEMENTS = 14;
    static const int BUDGET = 2;
    static const double APPROXIMATION_RATIO = 1 - 1 / std::exp(1.0);
    std::vector<std::vector<int>> sets_element = { { 1, 2 },
                                                   { 3, 4, 5, 6 },
                                                   { 7, 8, 9, 10, 11, 12, 13,
                                                     0 },
                                                   { 1, 3, 5, 7, 9, 11, 13 },
                                                   { 2, 4, 6, 8, 10, 12, 0 } };
    auto sets = paal::irange(5);
    std::vector<int> result;
    auto element_index = identity_functor{};
    auto element_to_weight = return_one_functor{};
    auto cost_of_set = return_one_functor{};
    auto covered_weight = paal::greedy::budgeted_maximum_coverage(
        sets, cost_of_set, make_array_to_functor(sets_element),
        std::back_inserter(result), element_index, BUDGET, element_to_weight);
    budgeted_maximum_coverage_result_check(
        result, sets_element, element_to_weight, cost_of_set, ELEMENTS, OPTIMAL,
        covered_weight, APPROXIMATION_RATIO, BUDGET);
}
