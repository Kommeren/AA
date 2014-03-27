/**
 * @file dreyfus_wagner_long_test.cpp
 * @brief
 * @author Maciej Andrejczuk
 * @version 1.0
 * @date 2013-08-04
 */

#include <boost/test/unit_test.hpp>

#include <vector>
#include <fstream>

#include "utils/logger.hpp"

#include "paal/steiner_tree/dreyfus_wagner.hpp"
#include "paal/data_structures/bimap.hpp"

#include "utils/read_steinlib.hpp"
#include "utils/sample_graph.hpp"
#include "utils/test_result_check.hpp"

using namespace paal;

BOOST_AUTO_TEST_CASE(steinlib_test) {
    std::vector<steiner_tree_test> data;
    LOGLN("READING INPUT...");
    read_steinlib_tests(data);
    int k = 0;
    for (const steiner_tree_test& test : data) {
        LOGLN("TEST " << test.test_name);
        auto dw = paal::steiner_tree::make_dreyfus_wagner(test.metric, test.terminals, test.steiner_points);
        dw.solve();
        int res = dw.get_cost();
        BOOST_CHECK_EQUAL(res, test.optimal);
        // Smaller tests only
        if (++k == 50) break;
    }
}
