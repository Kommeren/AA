//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file dreyfus_wagner_long_test.cpp
 * @brief
 * @author Maciej Andrejczuk
 * @version 1.0
 * @date 2013-08-04
 */

#include "test_utils/logger.hpp"
#include "test_utils/read_steinlib.hpp"
#include "test_utils/test_result_check.hpp"

#include "paal/steiner_tree/dreyfus_wagner.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/range/adaptor/sliced.hpp>

#include <vector>
#include <fstream>

using namespace paal;

BOOST_AUTO_TEST_CASE(dreyfus_wagner_steinlib_long_test) {
    std::vector<steiner_tree_test_with_metric> data;
    LOGLN("READING INPUT...");
    read_steinlib_tests(data);
    // Smaller tests only
    for (auto const &test : data | boost::adaptors::sliced(0, 50)) {
        LOGLN("TEST " << test.test_name);
        auto dw = paal::make_dreyfus_wagner(
            test.metric, test.terminals, test.steiner_points);
        dw.solve();
        int res = dw.get_cost();
        BOOST_CHECK_EQUAL(res, test.optimal);
    }
}
