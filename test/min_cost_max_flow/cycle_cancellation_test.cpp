#define BOOST_TEST_MODULE cycle_cancellation_test

#include <boost/test/unit_test.hpp>

#include <iostream>

#include "paal/min_cost_max_flow/cycle_cancellation.hpp"

#include "utils/sample_graph_undirected.hpp"


BOOST_AUTO_TEST_CASE(cycle_canceling_def_test) {
    unsigned s,t;
    boost::SampleGraphMetricsUndirected::Graph g 
        = boost::SampleGraphMetricsUndirected::getSampleGraph(s, t);

    boost::edmonds_karp_max_flow(g, s, t);
    boost::cycle_cancellation(g);

    int cost = boost::find_flow_cost(g);
    BOOST_CHECK(cost == 29);
}

BOOST_AUTO_TEST_CASE(cycle_canceling_test) {
    unsigned s,t;
    boost::SampleGraphMetricsUndirected::Graph g 
        = boost::SampleGraphMetricsUndirected::getSampleGraph(s, t);

    int N = num_vertices(g);
    std::vector<int> dist(N);
    std::vector<unsigned int> pre(N);

    boost::edmonds_karp_max_flow(g, s, t);
    boost::cycle_cancellation(g, boost::distance_map(&dist[0]).predecessor_map(&pre[0]));

    int cost = boost::find_flow_cost(g);
    BOOST_CHECK(cost == 29);
}
