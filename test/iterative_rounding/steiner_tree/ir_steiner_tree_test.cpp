//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file ir_steiner_tree_test.cpp
 * @brief
 * @author Maciej Andrejczuk, Piotr Godlewski, Piotr Wygocki
 * @version 1.0
 * @date 2013-02-04
 */

#include "utils/sample_graph.hpp"
#include "utils/logger.hpp"
#include "utils/test_result_check.hpp"

#include "paal/iterative_rounding/steiner_tree/steiner_tree.hpp"

#include <boost/test/unit_test.hpp>

#include <cmath>

using Vertex = int;
using Terminals = std::vector<int>;

BOOST_AUTO_TEST_SUITE(ir_steiner_tree)

static const double APPROXIMATION_RATIO = 1.39;

BOOST_AUTO_TEST_CASE(test_all_generator) {
    paal::ir::steiner_tree_all_generator strategy_all(5);

    Terminals terminals, steiner_vertices;
    std::vector<Vertex> result;
    // small graph
    auto metrics = sample_graphs_metrics::get_graph_metric_steiner();
    boost::tie(terminals, steiner_vertices) =
        sample_graphs_metrics::get_graph_steiner_vertices();

    auto status = paal::ir::steiner_tree_iterative_rounding(metrics, terminals,
            steiner_vertices, std::back_inserter(result), strategy_all);
    BOOST_CHECK_EQUAL(status, paal::lp::OPTIMAL);
    int cost = paal::ir::steiner_utils::count_cost(result, terminals, metrics);
    BOOST_CHECK(cost == 4);

    // check if algorithm doesn't modify supplied data
    BOOST_CHECK(boost::equal(terminals,
        sample_graphs_metrics::get_graph_steiner_vertices().first));
    BOOST_CHECK(boost::equal(steiner_vertices,
        sample_graphs_metrics::get_graph_steiner_vertices().second));
    auto m2 = sample_graphs_metrics::get_graph_metric_steiner();
    // TODO function comparing 2 metrics
    int n = m2.size();
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            BOOST_CHECK(metrics(i, j) == m2(i, j));
        }
    }

    // bigger graph
    result.clear();
    metrics = sample_graphs_metrics::get_graph_metric_steiner_bigger();
    boost::tie(terminals, steiner_vertices) =
        sample_graphs_metrics::get_graph_steiner_bigger_vertices();

    status = paal::ir::steiner_tree_iterative_rounding(metrics, terminals,
            steiner_vertices, std::back_inserter(result), strategy_all);
    BOOST_CHECK_EQUAL(status, paal::lp::OPTIMAL);
    cost = paal::ir::steiner_utils::count_cost(result, terminals, metrics);
    BOOST_CHECK(cost == 15);
}

template <typename Strategy>
void run_multiple_seed_tests(Strategy& strategy) {
    srand(0);

    Terminals terminals, steiner_vertices;
    std::vector<Vertex> result;

    // small graph
    auto metrics = sample_graphs_metrics::get_graph_metric_steiner();
    boost::tie(terminals, steiner_vertices) =
        sample_graphs_metrics::get_graph_steiner_vertices();

    int best_cost = std::numeric_limits<int>::max();
    for (int i : boost::irange(0, 5)) {
        srand(i);
        LOGLN("small graph, seed " << i);
        result.clear();
        auto status = paal::ir::steiner_tree_iterative_rounding(metrics, terminals,
                steiner_vertices, std::back_inserter(result), strategy);
        BOOST_CHECK_EQUAL(status, paal::lp::OPTIMAL);
        int cost = paal::ir::steiner_utils::count_cost(result, terminals, metrics);
        paal::assign_min(best_cost, cost);
        BOOST_CHECK(cost >= 4);
    }
    // warning: randomized algorithm, approximation ratio could be violated
    check_result(best_cost, 4, APPROXIMATION_RATIO);

    // bigger graph
    metrics = sample_graphs_metrics::get_graph_metric_steiner_bigger();
    boost::tie(terminals, steiner_vertices) =
        sample_graphs_metrics::get_graph_steiner_bigger_vertices();

    best_cost = std::numeric_limits<int>::max();
    for (int i : boost::irange(0, 5)) {
        LOGLN("big graph, seed " << i);
        srand(i);
        result.clear();
        auto status = paal::ir::steiner_tree_iterative_rounding(metrics, terminals,
                steiner_vertices, std::back_inserter(result), strategy);
        BOOST_CHECK_EQUAL(status, paal::lp::OPTIMAL);
        int cost = paal::ir::steiner_utils::count_cost(result, terminals, metrics);
        best_cost = std::min(best_cost, cost);
        BOOST_CHECK(cost >= 15);
    }
    // warning: randomized algorithm, approximation ratio could be violated
    check_result(best_cost, 15, APPROXIMATION_RATIO);
}

BOOST_AUTO_TEST_CASE(test_all_generator_seeds) {
    LOGLN("strategy_all");
    paal::ir::steiner_tree_all_generator strategy_all(5);
    run_multiple_seed_tests(strategy_all);
}

BOOST_AUTO_TEST_CASE(test_rand_generator) {
    LOGLN("strategy_rand");
    paal::ir::steiner_tree_random_generator strategy_rand(10, 5);
    run_multiple_seed_tests(strategy_rand);
}

BOOST_AUTO_TEST_CASE(test_smart_generator) {
    LOGLN("strategy_smart");
    paal::ir::steiner_tree_smart_generator strategy_smart(10, 5);
    run_multiple_seed_tests(strategy_smart);
}

BOOST_AUTO_TEST_CASE(test_graph_all_generator) {
    LOGLN("strategy_graph_all");
    srand(0);

    Terminals terminals, steiner_vertices;
    std::vector<Vertex> result;

    // small graph
    auto small_graph = sample_graphs_metrics::get_graph_steiner();
    auto metrics = sample_graphs_metrics::get_graph_metric_steiner();
    boost::tie(terminals, steiner_vertices) =
        sample_graphs_metrics::get_graph_steiner_vertices();

    int best_cost = std::numeric_limits<int>::max();
    for (int i : boost::irange(0, 5)) {
        auto strategy = paal::ir::make_steiner_tree_graph_all_generator<Vertex>(
            small_graph, terminals, 5);
        srand(i);
        LOGLN("small graph, seed " << i);
        result.clear();
        auto status = paal::ir::steiner_tree_iterative_rounding(metrics, terminals,
                steiner_vertices, std::back_inserter(result), strategy);
        BOOST_CHECK_EQUAL(status, paal::lp::OPTIMAL);
        int cost = paal::ir::steiner_utils::count_cost(result, terminals, metrics);
        best_cost = std::min(best_cost, cost);
        BOOST_CHECK(cost >= 4);
    }
    // warning: randomized algorithm, approximation ratio could be violated
    check_result(best_cost, 4, APPROXIMATION_RATIO);

    // bigger graph
    auto bigger_graph = sample_graphs_metrics::get_graph_steiner_bigger();
    metrics = sample_graphs_metrics::get_graph_metric_steiner_bigger();
    boost::tie(terminals, steiner_vertices) =
        sample_graphs_metrics::get_graph_steiner_bigger_vertices();

    best_cost = std::numeric_limits<int>::max();
    for (int i : boost::irange(0, 5)) {
        auto strategy = paal::ir::make_steiner_tree_graph_all_generator<Vertex>(
            bigger_graph, terminals, 5);
        LOGLN("big graph, seed " << i);
        srand(i);
        result.clear();
        auto status = paal::ir::steiner_tree_iterative_rounding(metrics, terminals,
                steiner_vertices, std::back_inserter(result), strategy);
        BOOST_CHECK_EQUAL(status, paal::lp::OPTIMAL);
        int cost = paal::ir::steiner_utils::count_cost(result, terminals, metrics);
        best_cost = std::min(best_cost, cost);
        BOOST_CHECK(cost >= 15);
    }
    // warning: randomized algorithm, approximation ratio could be violated
    check_result(best_cost, 15, APPROXIMATION_RATIO);
}

BOOST_AUTO_TEST_CASE(euclidean_metric_test) {
    using Points = std::vector<std::pair<int, int>>;

    srand(0);
    paal::ir::steiner_tree_random_generator strategy_rand(10, 5);
    paal::data_structures::euclidean_metric<int> em;
    Points terminals, steiner_vertices, result;

    std::tie(em, terminals, steiner_vertices) =
        sample_graphs_metrics::get_euclidean_steiner_sample();

    auto status = paal::ir::steiner_tree_iterative_rounding(em, terminals, steiner_vertices,
                std::back_inserter(result), strategy_rand);
    BOOST_CHECK_EQUAL(status, paal::lp::OPTIMAL);
    auto cost = paal::ir::steiner_utils::count_cost(result, terminals, em);

    BOOST_CHECK_EQUAL(result.size(), std::size_t(1));
    BOOST_CHECK(result.front() == std::make_pair(1, 1));
    BOOST_CHECK_CLOSE(cost, 4 * std::sqrt(2), 1e-6);
}

BOOST_AUTO_TEST_SUITE_END()
