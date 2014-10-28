//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
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
#include <boost/range/adaptor/sliced.hpp>

#include <fstream>
#include <vector>

using namespace paal;
using namespace paal::ir;

using Vertex = int;
using Dist = int;
using Terminals = std::vector<int>;
using Metric = GraphMT;

static const double APPROXIMATION_RATIO = 1.39;

template <typename OracleStrategy = paal::lp::random_violated_separation_oracle,
          typename Strategy>
void run_test(const steiner_tree_test_with_metric& test, const Strategy& strategy) {
    std::vector<int> result;
    auto status =
        paal::ir::steiner_tree_iterative_rounding<OracleStrategy>(
            test.metric, test.terminals, test.steiner_points,
            std::back_inserter(result), strategy);
    BOOST_CHECK_EQUAL(status, paal::lp::OPTIMAL);
    int res = paal::ir::steiner_utils::count_cost(result, test.terminals, test.metric);

    LOG("RES " << res << "\n");
    LOG("APPROXIMATION_RATIO:" << double(res) / double(test.optimal) << "\n");
    check_result(res, test.optimal, APPROXIMATION_RATIO);
}

BOOST_AUTO_TEST_CASE(ir_steiner_tree_long_test) {
    std::vector<steiner_tree_test_with_metric> data;
    read_steinlib_tests(data);
    paal::ir::steiner_tree_random_generator strategy_rand(50, 5);
    // First tests only
    for (const auto &test : data | boost::adaptors::sliced(0, 10)) {
        LOG("TEST " << test.test_name << "\n");
        LOG("OPT " << test.optimal << "\n");

        for (int i : boost::irange(0, 5)) {
            LOGLN("random violated, seed " << i);
            srand(i);
            run_test(test, strategy_rand);

            LOGLN("random violated + graph strategy, seed " << i);
            srand(i);
            auto strategy_graph = paal::ir::make_steiner_tree_graph_all_generator<Vertex>(
                    test.graph, test.terminals, 5);
            run_test(test, strategy_graph);
        }

        LOGLN("most violated");
        run_test<paal::lp::max_violated_separation_oracle>(test, strategy_rand);

        LOGLN("first violated");
        run_test<paal::lp::first_violated_separation_oracle>(test, strategy_rand);
    }
}
