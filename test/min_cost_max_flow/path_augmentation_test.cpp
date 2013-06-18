#define BOOST_TEST_MODULE path_augmentation_test

#include <boost/test/unit_test.hpp>
#include <iostream>

#include "paal/min_cost_max_flow/path_augmentation.hpp"
#include "paal/min_cost_max_flow/find_flow_cost.hpp"

#include "utils/sample_graph_undirected.hpp"


BOOST_AUTO_TEST_CASE(path_augmentation_def_test) {
    unsigned s,t;
    boost::SampleGraphMetricsUndirected::Graph g 
        = boost::SampleGraphMetricsUndirected::getSampleGraph(s, t);

    boost::path_augmentation(g, s, t);

    int cost =  boost::find_flow_cost(g);
    BOOST_CHECK(cost == 29);
}

BOOST_AUTO_TEST_CASE(path_augmentation_test) {
    unsigned s,t;
    boost::SampleGraphMetricsUndirected::Graph g 
        = boost::SampleGraphMetricsUndirected::getSampleGraph(s, t);

    int N = num_vertices(g);
    std::vector<int> dist(N);
    std::vector<int> pre(N);

    boost::path_augmentation(g, s, t, boost::distance_map(&dist[0]).predecessor_map(&pre[0]));

    int cost =  boost::find_flow_cost(g);
    BOOST_CHECK(cost == 29);
}

