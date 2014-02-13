/**
 * @file set_cover_test.cpp
 * @brief
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2014-02-17
 */

#include "test_utils/set_cover_check.hpp"
#include "test_utils/test_result_check.hpp"
#include "test_utils/logger.hpp"

#include "paal/greedy/set_cover/set_cover.hpp"
#include "paal/utils/functors.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/range/irange.hpp>

#include <vector>
#include <iterator>

BOOST_AUTO_TEST_CASE(SetCover) {
    const int OPTIMAL = 2;
    std::vector<std::vector<int>> sets_element = { { 1, 2 },
                                                   { 3, 4, 5, 6 },
                                                   { 7, 8, 9, 10, 11, 12, 13,
                                                     14 },
                                                   { 1, 3, 5, 7, 9, 11, 13 },
                                                   { 2, 4, 6, 8, 10, 12, 0 } };
    auto costs = paal::utils::return_one_functor();
    auto sets = boost::irange(0, 5);
    auto set_to_elements = paal::utils::make_array_to_functor(sets_element);
    std::vector<int> result;
    auto element_index = paal::utils::identity_functor{};
    auto cost =
        paal::greedy::set_cover(sets, costs, set_to_elements,
                                std::back_inserter(result), element_index);
    double approximation_ratio =
        set_cover_result_check(sets, set_to_elements, result);
    check_result(cost, OPTIMAL, approximation_ratio);
}
