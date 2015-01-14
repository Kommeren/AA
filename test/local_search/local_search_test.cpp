//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file local_search_test.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-04
 */

#include "test_utils/logger.hpp"
#include "test_utils/simple_single_local_search_components.hpp"

#include "paal/local_search/custom_components.hpp"

#include <boost/test/unit_test.hpp>

#include <vector>

namespace ls = paal::local_search;
using namespace paal;

namespace {
ON_LOG(int i = 0;)

bool log_action(int s) {
    LOGLN("f(" << s << ") \t" << f(s) << " after " << ++i);
    return true;
}

} // anonymous namespace
BOOST_AUTO_TEST_SUITE(local_search)

BOOST_AUTO_TEST_CASE(local_test) {
    // printing
    int solution(0);
    LOGLN("f(" << solution << ") \t" << f(solution));
    utils::always_false nop;

    // first_improving
    BOOST_CHECK(ls::local_search(solution, ls::first_improving_strategy{},
                                 log_action, nop, search_comps()));
    BOOST_CHECK_EQUAL(solution, 6);


    solution = 0;
    // best improving
    BOOST_CHECK(ls::local_search(solution, ls::best_improving_strategy{}, log_action,
                                 nop, search_comps()));
    BOOST_CHECK_EQUAL(solution, 6);

    solution = 0;

    //best
    auto stop_after_five_iterations = paal::utils::make_not_functor(
        paal::local_search::stop_condition_count_limit{ 5 });
    auto on_success =
        paal::utils::make_and_functor(log_action, stop_after_five_iterations);

    // search
    BOOST_CHECK(ls::best(solution, on_success, search_comps{}));
    BOOST_CHECK_EQUAL(solution, 6);

}

BOOST_AUTO_TEST_SUITE_END()
