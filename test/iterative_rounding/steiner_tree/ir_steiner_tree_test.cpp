/**
 * @file ir_steiner_tree_test.cpp
 * @brief
 * @author Maciej Andrejczuk
 * @version 1.0
 * @date 2013-02-04
 */
#define BOOST_TEST_MODULE ir_steiner_tree_test

#include <boost/test/unit_test.hpp>

#include "iterative_rounding/steiner_tree/sample_graph.hpp"
#include "paal/iterative_rounding/steiner_tree/steiner_tree.hpp"

typedef int Vertex;
typedef int Dist;
typedef typename paal::data_structures::GraphMetric<Graph, Dist> UserMetric;
typedef std::vector<int> Terminals;

BOOST_AUTO_TEST_CASE(testAllGenerator) {
    paal::ir::AllGenerator strategyAll(5);

    Terminals terminals, steinerVertices;
    std::vector<Vertex> result;
    // small graph
    UserMetric metrics(createSmallGraph());
    boost::tie(terminals, steinerVertices) = getSmallGraphVertices();
    paal::ir::solve_steiner_tree(metrics, terminals, steinerVertices,
            std::back_inserter(result), strategyAll);
    int cost = paal::ir::SteinerUtils::countCost(result, terminals, metrics);
    BOOST_CHECK(cost == 4);

    // check if algorithm doesn't modify supplied data
    BOOST_CHECK(terminals.size() == getSmallGraphVertices().first.size());
    BOOST_CHECK(steinerVertices.size() == getSmallGraphVertices().second.size());
    UserMetric m2(createSmallGraph());
    int n = m2.size();
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            BOOST_CHECK(metrics(i, j) == m2(i, j));
        }
    }

    // bigger graph
    result.clear();
    metrics = UserMetric(createBiggerGraph());
    boost::tie(terminals, steinerVertices) = getBiggerGraphVertices();
    paal::ir::solve_steiner_tree(metrics, terminals, steinerVertices,
            std::back_inserter(result), strategyAll);
    cost = paal::ir::SteinerUtils::countCost(result, terminals, metrics);
    BOOST_CHECK(cost == 15);
}

BOOST_AUTO_TEST_CASE(testRandGenerator) {
    paal::ir::RandomGenerator strategyRand(10, 5);

    Terminals terminals, steinerVertices;
    std::vector<Vertex> result;
    // small graph
    UserMetric metrics(createSmallGraph());
    boost::tie(terminals, steinerVertices) = getSmallGraphVertices();
    paal::ir::solve_steiner_tree(metrics, terminals, steinerVertices,
            std::back_inserter(result), strategyRand);
    int cost = paal::ir::SteinerUtils::countCost(result, terminals, metrics);
    BOOST_CHECK(cost == 4);

    // bigger graph
    result.clear();
    metrics = UserMetric(createBiggerGraph());
    boost::tie(terminals, steinerVertices) = getBiggerGraphVertices();
    paal::ir::solve_steiner_tree(metrics, terminals, steinerVertices,
            std::back_inserter(result), strategyRand);
    cost = paal::ir::SteinerUtils::countCost(result, terminals, metrics);
    BOOST_CHECK(cost == 15);
}
