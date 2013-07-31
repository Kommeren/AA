#define BOOST_TEST_MODULE cycle_canceling_test

#include <boost/test/unit_test.hpp>

#include "paal/min_cost_max_flow/cycle_canceling.hpp"
#include "boost/graph/edmonds_karp_max_flow.hpp"

#include "min_cost_max_flow_utils.hpp"


BOOST_AUTO_TEST_CASE(cycle_canceling_def_test) {
    unsigned s,t;
    boost::SampleGraphMetricsUndirected::Graph g 
        = boost::SampleGraphMetricsUndirected::getSampleGraph(s, t);

    boost::edmonds_karp_max_flow(g, s, t);
    boost::cycle_canceling(g);

    int cost = boost::find_flow_cost(g);
    BOOST_CHECK(cost == 29);
}

BOOST_AUTO_TEST_CASE(cycle_canceling_test) {
    unsigned s,t;
    typedef boost::SampleGraphMetricsUndirected::Graph Graph;
    Graph g = boost::SampleGraphMetricsUndirected::getSampleGraph(s, t);

    int N = num_vertices(g);
    std::vector<int> dist(N);
    typedef typename boost::graph_traits<Graph>::edge_descriptor edge_descriptor;
    std::vector<edge_descriptor> pred(N);

    boost::edmonds_karp_max_flow(g, s, t);
    boost::cycle_canceling(g, boost::distance_map(&dist[0]).predecessor_map(&pred[0]).vertex_index_map(boost::identity_property_map()));

    int cost = boost::find_flow_cost(g);
    BOOST_CHECK(cost == 29);
}
