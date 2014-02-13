/**
 * @file maximum_coverage_test.cpp
 * @brief
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2014-02-17
 */

#include "test_utils/logger.hpp"
#include "test_utils/test_result_check.hpp"

#include "paal/utils/functors.hpp"
#include "paal/greedy/set_cover/maximum_coverage.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/range/irange.hpp>

#include <vector>
#include <iterator>
#include <cmath>

BOOST_AUTO_TEST_CASE(MaximumCoverage) {
    LOGLN("MaximumCoverage");
    const int OPTIMAL = 14;
    const int NUMBER_OF_SETS = 2;
    const double APPROXIMATION_RATIO = 1 - 1 / std::exp(1.0);
    std::vector<std::vector<int>> sets_element = { { 1, 2 },
                                                   { 3, 4, 5, 6 },
                                                   { 7, 8, 9, 10, 11, 12, 13,
                                                     0 },
                                                   { 1, 3, 5, 7, 9, 11, 13 },
                                                   { 2, 4, 6, 8, 10, 12, 0 } };
    auto sets = boost::irange(0, 5);
    std::vector<int> result;
    auto element_index = paal::utils::identity_functor{};
    auto cost = paal::greedy::maximum_coverage(
        sets, paal::utils::make_array_to_functor(sets_element),
        std::back_inserter(result), element_index, NUMBER_OF_SETS);
    check_result(cost, OPTIMAL, APPROXIMATION_RATIO,
                 paal::utils::greater_equal());
}
