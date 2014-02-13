/**
 * @file budgeted_maximum_coverage_long_test.cpp
 * @brief
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2014-02-17
 */

#include "paal/greedy/set_cover/budgeted_maximum_coverage.hpp"
#include "paal/utils/functors.hpp"
#include "test_utils/logger.hpp"
#include "test_utils/test_result_check.hpp"
#include "test_utils/budgeted_maximum_coverage_check.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/range/irange.hpp>
#include <boost/range/algorithm_ext/iota.hpp>

#include <vector>
#include <cstdlib>
#include <algorithm>
#include <iterator>
#include <cmath>

BOOST_AUTO_TEST_CASE(budgeted_maximum_coverage_long) {
    LOGLN("budgeted_maximum_coverage_long");
    const double APPROXIMATION_RATIO = 1 - 1 / std::exp(1.0);
    const int ELEMENTS = 100;
    const int SETSIZE = 5;

    std::vector<int> weight(ELEMENTS);
    std::vector<int> solution(ELEMENTS);
    int optimal = 0;
    srand(42);
    for (int i = 0; i < ELEMENTS; ++i) {
        weight[i] = rand() % 1000;
        optimal += weight[i];
    }
    boost::iota(solution, 0);
    std::random_shuffle(solution.begin(), solution.end());
    std::vector<std::vector<int>> sets_element;
    std::vector<int> cost_of_set;
    for (int i = 0; i < ELEMENTS; i += SETSIZE) {
        for (int j = 0; j < 4; ++j) {
            std::vector<int> set;
            int set_cost = 0;
            for (int k = 0; k < SETSIZE; ++k) {
                if (j == 3) {
                    set.push_back(solution[i + k]);
                    set_cost += weight[solution[i + k]];
                } else {
                    int el;
                    bool copy;
                    do {
                        copy = false;
                        el = rand() % ELEMENTS;
                        for (auto i : set)
                            if (i == el) copy = true;
                    } while (copy);
                    set.push_back(el);
                    set_cost += weight[el];
                }
            }
            sets_element.push_back(set);
            cost_of_set.push_back(set_cost);
        }
    }
    auto sets = boost::irange(0, int(sets_element.size()));
    std::vector<int> result;
    auto element_index = paal::utils::identity_functor{};
    auto set_to_cost= paal::utils::make_array_to_functor(cost_of_set);
    auto set_to_weight = paal::utils::make_array_to_functor(weight);
    auto budget=optimal;
    auto weight_solution = paal::greedy::budgeted_maximum_coverage(
        sets, set_to_cost,
        paal::utils::make_array_to_functor(sets_element),
        std::back_inserter(result), element_index, budget, set_to_weight
        );
    budgeted_maximum_coverage_result_check(
        result, sets_element, set_to_weight, set_to_cost, ELEMENTS, optimal,
        weight_solution, APPROXIMATION_RATIO, budget);
}
