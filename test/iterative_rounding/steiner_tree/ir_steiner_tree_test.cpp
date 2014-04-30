/**
 * @file ir_steiner_tree_test.cpp
 * @brief
 * @author Maciej Andrejczuk
 * @version 1.0
 * @date 2013-02-04
 */

#include "iterative_rounding/steiner_tree/sample_graph.hpp"

#include "paal/iterative_rounding/steiner_tree/steiner_tree.hpp"

#include <boost/test/unit_test.hpp>


typedef int Vertex;
typedef int Dist;
typedef typename paal::data_structures::graph_metric<Graph, Dist> UserMetric;
typedef std::vector<int> Terminals;

BOOST_AUTO_TEST_CASE(testall_generator) {
    paal::ir::all_generator strategyAll(5);

    Terminals terminals, steinerVertices;
    std::vector<Vertex> result;
    // small graph
    UserMetric metrics(create_small_graph());
    boost::tie(terminals, steinerVertices) = get_small_graph_vertices();
    paal::ir::steiner_tree_iterative_rounding(metrics, terminals, steinerVertices,
            std::back_inserter(result), strategyAll);
    int cost = paal::ir::steiner_utils::count_cost(result, terminals, metrics);
    BOOST_CHECK(cost == 4);

    // check if algorithm doesn't modify supplied data
    BOOST_CHECK(terminals.size() == get_small_graph_vertices().first.size());
    BOOST_CHECK(steinerVertices.size() == get_small_graph_vertices().second.size());
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
    boost::tie(terminals, steinerVertices) = get_bigger_graph_vertices();
    paal::ir::steiner_tree_iterative_rounding(metrics, terminals, steinerVertices,
            std::back_inserter(result), strategyAll);
    cost = paal::ir::steiner_utils::count_cost(result, terminals, metrics);
    BOOST_CHECK(cost == 15);
}

BOOST_AUTO_TEST_CASE(testRandGenerator) {
    srand(0);
    paal::ir::random_generator strategyRand(10, 5);

    Terminals terminals, steinerVertices;
    std::vector<Vertex> result;
    // small graph
    UserMetric metrics(create_small_graph());
    boost::tie(terminals, steinerVertices) = get_small_graph_vertices();
    paal::ir::steiner_tree_iterative_rounding(metrics, terminals, steinerVertices,
            std::back_inserter(result), strategyRand);
    int cost = paal::ir::steiner_utils::count_cost(result, terminals, metrics);
    BOOST_CHECK(cost == 4);

    // bigger graph
    result.clear();
    metrics = UserMetric(create_bigger_graph());
    boost::tie(terminals, steinerVertices) = get_bigger_graph_vertices();
    paal::ir::steiner_tree_iterative_rounding(metrics, terminals, steinerVertices,
            std::back_inserter(result), strategyRand);
    cost = paal::ir::steiner_utils::count_cost(result, terminals, metrics);
    BOOST_CHECK(cost == 15);
}
