//=======================================================================
// Copyright (c) 2014 Piotr Smulewicz
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file k_center_test.cpp
 * @brief
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2014-01-28
 */

#include "utils/test_result_check.hpp"
#include "greedy/k_center/in_balls.hpp"

#include "paal/data_structures/metric/basic_metrics.hpp"
#include "paal/greedy/k_center/k_center.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/range/irange.hpp>

BOOST_AUTO_TEST_CASE(KCenter) {
    const int NUM_CENTERS = 3;
    const int NUM_ITEMS = 6;
    const double OPTIMAL = 1;
    const double APPROXIMATION_RATIO = 2;
    auto metric = [](int a, int b) { return 0.1 + abs(a - b) * 0.9; };
    auto items = boost::irange(0, NUM_ITEMS);
    std::vector<int> centers;
    // solution
    double radius = paal::greedy::kCenter(metric, NUM_CENTERS, items.begin(),
                                          items.end(), back_inserter(centers));
    LOGLN("Radius " << radius);
    LOGLN("Centers:");
    LOG_COPY_RANGE_DEL(centers, " ");
    LOGLN("");
    BOOST_CHECK_EQUAL(centers.size(), NUM_CENTERS);
    check_result(radius, OPTIMAL, APPROXIMATION_RATIO);
    paal::in_balls(items, centers, metric, radius);
}
