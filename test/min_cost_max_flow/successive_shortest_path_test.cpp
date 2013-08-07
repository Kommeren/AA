#define BOOST_TEST_MODULE successive_shortest_path_test

#include <boost/test/unit_test.hpp>

#include "paal/min_cost_max_flow/successive_shortest_path.hpp"
#include "paal/min_cost_max_flow/find_flow_cost.hpp"

#include "min_cost_max_flow_utils.hpp"


BOOST_AUTO_TEST_CASE(path_augmentation_def_test) {
    unsigned s,t;
    boost::SampleGraphMetricsUndirected::Graph g 
        = boost::SampleGraphMetricsUndirected::getSampleGraph(s, t);

    boost::successive_shortest_path(g, s, t);

    int cost =  boost::find_flow_cost(g);
    BOOST_CHECK(cost == 29);
}

BOOST_AUTO_TEST_CASE(path_augmentation_test) {
    unsigned s,t;
    typedef boost::SampleGraphMetricsUndirected::Graph Graph;
    Graph g = boost::SampleGraphMetricsUndirected::getSampleGraph(s, t);

    int N = boost::num_vertices(g);
    std::vector<int> dist(N);
    std::vector<int> dist_prev(N);
    typedef typename boost::graph_traits<Graph>::edge_descriptor edge_descriptor;
    std::vector<edge_descriptor> pred(N);
        

    boost::successive_shortest_path(g, s, t, 
            boost::distance_map(&dist[0]).
            predecessor_map(&pred[0]).
            distance_zero(&dist_prev[0]).
            vertex_index_map(boost::identity_property_map()));

    int cost =  boost::find_flow_cost(g);
    BOOST_CHECK(cost == 29);
}

