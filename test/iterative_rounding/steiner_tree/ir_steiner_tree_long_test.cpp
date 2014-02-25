/**
 * @file ir_steiner_tree_long_test.cpp
 * @brief
 * @author Maciej Andrejczuk
 * @version 1.0
 * @date 2013-02-04
 */

#include <boost/test/unit_test.hpp>

#include <vector>
#include <fstream>

#include "utils/logger.hpp"
#include "paal/iterative_rounding/steiner_tree/steiner_tree.hpp"
#include "utils/read_steinlib.hpp"
#include "utils/test_result_check.hpp"

using namespace paal;
using namespace paal::ir;

typedef int Vertex;
typedef int Dist;
typedef std::vector<int> Terminals;
typedef GraphMT Metric;

BOOST_AUTO_TEST_CASE(steiner_tree_long_test) {
    std::vector<SteinerTreeTest> data;
    readSTEINLIBtests(data);
    int k = 0;
    for (const SteinerTreeTest& test : data) {
        LOG("TEST " << test.testName << "\n");
        LOG("OPT " << test.optimal << "\n");

        paal::ir::RandomGenerator strategyRand(50, 5);
        std::vector<int> result;
        paal::ir::solve_steiner_tree(test.metric, test.terminals, test.steinerPoints,
                std::back_inserter(result), strategyRand);
        int res = paal::ir::SteinerUtils::countCost(result, test.terminals, test.metric);

        check_result(res,test.optimal,1.39);

        // First tests only
        if (++k == 10) break;
    }
}
