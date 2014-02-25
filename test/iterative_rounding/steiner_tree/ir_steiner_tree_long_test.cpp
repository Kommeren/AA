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

template <template <typename> class OracleStrategy>
void runTest(const SteinerTreeTest & test) {
    paal::ir::RandomGenerator strategyRand(50, 5);
    std::vector<int> result;
    paal::ir::steiner_tree_iterative_rounding<SteinerTreeOracle<OracleStrategy>>(
            test.metric, test.terminals, test.steinerPoints,
            std::back_inserter(result), strategyRand);
        ON_LOG(int res = )paal::ir::SteinerUtils::countCost(result, test.terminals, test.metric);

        LOG("RES " << res << "\n");
        LOG("APPROXIMATION_RATIO:" << double(res) / double(test.optimal) << "\n");
}

BOOST_AUTO_TEST_CASE(steiner_long_test) {
    std::vector<SteinerTreeTest> data;
    readSTEINLIBtests(data);
    int k = 0;
    for (const SteinerTreeTest& test : data) {
        LOG("TEST " << test.testName << "\n");
        LOG("OPT " << test.optimal << "\n");

        for (int i : boost::irange(0, 5)) {
            LOGLN("random violated, seed " << i);
            srand(i);
            runTest<paal::lp::RandomViolatedSeparationOracle>(test);
        }

        LOGLN("most violated");
        runTest<paal::lp::MostViolatedSeparationOracle>(test);

        LOGLN("first violated");
        runTest<paal::lp::FirstViolatedSeparationOracle>(test);

        // First tests only
        if (++k == 10) break;
    }
}
