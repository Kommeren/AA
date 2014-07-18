/**
 * @file ir_steiner_tree_test.cpp
 * @brief
 * @author Maciej Andrejczuk, Piotr Godlewski, Piotr Wygocki
 * @version 1.0
 * @date 2013-02-04
 */

#include "iterative_rounding/steiner_tree/sample_graph.hpp"
#include "utils/sample_graph.hpp"
#include "utils/logger.hpp"

#include "paal/iterative_rounding/steiner_tree/steiner_tree.hpp"

#include <boost/test/unit_test.hpp>


using Vertex = int;
using Dist = int;
using UserMetric = typename paal::data_structures::graph_metric<Graph, Dist>;
using Terminals = std::vector<int>;

BOOST_AUTO_TEST_SUITE(ir_steiner_tree)

BOOST_AUTO_TEST_CASE(test_all_generator) {
    paal::ir::all_generator strategy_all(5);

    Terminals terminals, steiner_vertices;
    std::vector<Vertex> result;
    // small graph
    UserMetric metrics(create_small_graph());
    boost::tie(terminals, steiner_vertices) = get_small_graph_vertices();
    paal::ir::steiner_tree_iterative_rounding(metrics, terminals, steiner_vertices,
            std::back_inserter(result), strategy_all);
    int cost = paal::ir::steiner_utils::count_cost(result, terminals, metrics);
    BOOST_CHECK(cost == 4);

    // check if algorithm doesn't modify supplied data
    BOOST_CHECK(terminals.size() == get_small_graph_vertices().first.size());
    BOOST_CHECK(steiner_vertices.size() == get_small_graph_vertices().second.size());
    UserMetric m2(create_small_graph());
    int n = m2.size();
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            BOOST_CHECK(metrics(i, j) == m2(i, j));
        }
    }

    // bigger graph
    result.clear();
    metrics = UserMetric(create_bigger_graph());
    boost::tie(terminals, steiner_vertices) = get_bigger_graph_vertices();
    paal::ir::steiner_tree_iterative_rounding(metrics, terminals, steiner_vertices,
            std::back_inserter(result), strategy_all);
    cost = paal::ir::steiner_utils::count_cost(result, terminals, metrics);
    BOOST_CHECK(cost == 15);
}

BOOST_AUTO_TEST_CASE(test_rand_generator) {
    srand(0);
    paal::ir::random_generator strategy_rand(10, 5);

    Terminals terminals, steiner_vertices;
    std::vector<Vertex> result;

    // small graph
    UserMetric metrics(create_small_graph());
    boost::tie(terminals, steiner_vertices) = get_small_graph_vertices();
    for (int i : boost::irange(0, 5)) {
        srand(i);
        LOGLN("small graph, seed " << i);
        result.clear();
        paal::ir::steiner_tree_iterative_rounding(metrics, terminals, steiner_vertices,
                std::back_inserter(result), strategy_rand);
        int cost = paal::ir::steiner_utils::count_cost(result, terminals, metrics);
        BOOST_CHECK(cost == 4);
    }

    // bigger graph
    metrics = UserMetric(create_bigger_graph());
    boost::tie(terminals, steiner_vertices) = get_bigger_graph_vertices();
    for (int i : boost::irange(0, 5)) {
        LOGLN("big graph, seed " << i);
        srand(i);
        result.clear();
        paal::ir::steiner_tree_iterative_rounding(metrics, terminals, steiner_vertices,
                std::back_inserter(result), strategy_rand);
        int cost = paal::ir::steiner_utils::count_cost(result, terminals, metrics);
        BOOST_CHECK(cost == 15);
    }
}

BOOST_AUTO_TEST_CASE(euclidean_metric_test) {
    using Points = std::vector<std::pair<int, int>>;

    srand(0);
    paal::ir::random_generator strategy_rand(10, 5);
    paal::data_structures::euclidean_metric<int> em;
    Points terminals, steiner_vertices, result;

    std::tie(em, terminals, steiner_vertices) = sample_graphs_metrics::get_euclidean_steiner_sample();

    paal::ir::steiner_tree_iterative_rounding(em, terminals, steiner_vertices,
                std::back_inserter(result), strategy_rand);
    int cost = paal::ir::steiner_utils::count_cost(result, terminals, em);

    BOOST_CHECK_EQUAL(result.size(), std::size_t(1));
    BOOST_CHECK(result.front() == std::make_pair(1, 1));
    BOOST_CHECK_EQUAL(cost, 4);
}

BOOST_AUTO_TEST_SUITE_END()
