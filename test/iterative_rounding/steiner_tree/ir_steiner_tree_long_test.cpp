/**
 * @file ir_steiner_tree_long_test.cpp
 * @brief
 * @author Maciej Andrejczuk
 * @version 1.0
 * @date 2013-02-04
 */

#include "utils/logger.hpp"
#include "utils/read_steinlib.hpp"
#include "utils/test_result_check.hpp"

#include "paal/iterative_rounding/steiner_tree/steiner_tree.hpp"

#include <boost/test/unit_test.hpp>

#include <vector>
#include <fstream>

using namespace paal;
using namespace paal::ir;

typedef int Vertex;
typedef int Dist;
typedef std::vector<int> Terminals;
typedef GraphMT Metric;

template <template <typename> class OracleStrategy>
void run_test(const steiner_tree_test & test) {
    paal::ir::random_generator strategyRand(50, 5);
    std::vector<int> result;
    paal::ir::steiner_tree_iterative_rounding<steiner_treeOracle<OracleStrategy>>(
            test.metric, test.terminals, test.steiner_points,
            std::back_inserter(result), strategyRand);
        ON_LOG(int res = )paal::ir::steiner_utils::count_cost(result, test.terminals, test.metric);

        LOG("RES " << res << "\n");
        LOG("APPROXIMATION_RATIO:" << double(res) / double(test.optimal) << "\n");
}

BOOST_AUTO_TEST_CASE(steiner_long_test) {
    std::vector<steiner_tree_test> data;
    read_steinlib_tests(data);
    int k = 0;
    for (const steiner_tree_test& test : data) {
        LOG("TEST " << test.test_name << "\n");
        LOG("OPT " << test.optimal << "\n");

        for (int i : boost::irange(0, 5)) {
            LOGLN("random violated, seed " << i);
            srand(i);
            run_test<paal::lp::random_violated_separation_oracle>(test);
        }

        LOGLN("most violated");
        run_test<paal::lp::most_violated_separation_oracle>(test);

        LOGLN("first violated");
        run_test<paal::lp::first_violated_separation_oracle>(test);

        // First tests only
        if (++k == 10) break;
    }
}
