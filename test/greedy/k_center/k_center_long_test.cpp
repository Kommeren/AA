//=======================================================================
// Copyright (c) 2014 Piotr Smulewicz
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file k_center_long_test.cpp
 * @brief
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2014-01-29
 */

#include "test_utils/test_result_check.hpp"

#include "greedy/k_center/in_balls.hpp"

#include "paal/data_structures/metric/basic_metrics.hpp"
#include "paal/greedy/k_center/k_center.hpp"
#include "paal/utils/irange.hpp"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(KCenter) {
    // performance test
    const int NUM_CENTERS = 1000;
    const int NUM_ITEMS = 1000 * 1000;
    const int P1 = 15469313;
    const int P2 = 10554857;
    const int P3 = 4241099;
    const double APPROXIMATION_RATIO = 2;
    auto metric = [ = ](int a, int b) {
        if (a > b) std::swap(a, b);
        if (a == b) return 0;
        return ((a * a * P3 + b * P2) % P1 +
                P1 * (1 + 100 * (((b - a) % NUM_CENTERS) != 0)));
    };
    auto items = paal::irange(NUM_ITEMS);
    std::vector<int> centers;
    // solution
    int radius = paal::greedy::kCenter(metric, NUM_CENTERS, items.begin(),
                                       items.end(), back_inserter(centers));
    BOOST_CHECK_EQUAL(centers.size(), NUM_CENTERS);
    check_result(radius, P1, APPROXIMATION_RATIO, paal::utils::less_equal(), 0,
                 "lower bound ", "upper bound for approximation ratio ");
    paal::in_balls(items, centers, metric, radius);
}
