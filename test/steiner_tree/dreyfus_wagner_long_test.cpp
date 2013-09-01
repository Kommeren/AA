/**
 * @file dreyfus_wagner_long_test.cpp
 * @brief 
 * @author Maciej Andrejczuk
 * @version 1.0
 * @date 2013-08-04
 */

#define BOOST_TEST_MODULE dreyfus_wagner_long

#include <boost/test/unit_test.hpp>

#include <vector>
#include <fstream>

#include "utils/logger.hpp"

#include "paal/steiner_tree/dreyfus_wagner.hpp"
#include "paal/data_structures/bimap.hpp"

#include "utils/read_steinlib.hpp"
#include "utils/sample_graph.hpp"

using namespace paal;

BOOST_AUTO_TEST_CASE(steinlib_test) {
    std::vector<SteinerTreeTest> data;
    LOG("READING INPUT...");
    readSTEINLIBtests(data);
    int k = 0;
    for (const SteinerTreeTest& test : data) {
        LOG("TEST " << test.testName);
        LOG("OPT " << test.optimal);
        auto dw = paal::steiner_tree::make_DreyfusWagner(test.metric, test.terminals, test.steinerPoints);
        dw.solve();
        int res = dw.getCost();
        BOOST_CHECK_EQUAL(res, test.optimal);
        // Smaller tests only
        if (++k == 50) break;
    }
}
