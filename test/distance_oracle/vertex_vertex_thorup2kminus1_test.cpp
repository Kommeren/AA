//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
* @file vertex_vertex_thorup2kminus1_test.cpp
* @brief
* @author Jakub Oćwieja
* @version 1.0
* @date 2014-05-11
*/
#include "paal/distance_oracle/vertex_vertex/thorup_2kminus1.hpp"
#include "paal/data_structures/metric/graph_metrics.hpp"

#include "test_utils/sample_graph.hpp"
#include "test_utils/test_result_check.hpp"

#include "paal/utils/irange.hpp"
#include "paal/utils/indexed_range.hpp"

#include <boost/graph/copy.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/test/unit_test.hpp>

using namespace paal;
using SGM = sample_graphs_metrics;

template <typename Graph, typename Oracle, typename GraphMetric, typename IndexMap>
void perform_full_check_helper(const Graph& g, const Oracle& oracle, const GraphMetric& gm, double ratio, IndexMap index) {

    for (auto v: boost::make_iterator_range(vertices(g))) {
        for (auto u: boost::make_iterator_range(vertices(g))) {
            check_result(oracle(u, v), gm(index[u], index[v]), ratio);
        }
    }
}

template <typename Graph, typename Oracle>
void perform_full_check(const Graph& g, const Oracle& oracle, double ratio) {
    using DistanceType = typename boost::property_map<Graph, boost::edge_weight_t>::value_type;
    data_structures::graph_metric<Graph, DistanceType> gm(g);

    perform_full_check_helper(g, oracle, gm, ratio, get(boost::vertex_index, g));
}

template <typename Graph, typename Oracle, typename IndexMap>
void perform_full_check(const Graph& g, const Oracle& oracle, double ratio, IndexMap index) {
    using DistanceType = typename boost::property_map<Graph, boost::edge_weight_t>::value_type;

    // TODO Extend graph_metric interface to accept graphs with external parameters (in this case index_map)
    SGM::Graph gg;
    boost::copy_graph(g, gg, vertex_index_map(index));
    data_structures::graph_metric<SGM::Graph, DistanceType> gm(gg);

    perform_full_check_helper(g, oracle, gm, ratio, index);
}

BOOST_AUTO_TEST_CASE( vv_thorup2kminus1_star_test ) {
    auto g = SGM::get_star_medium();

    for (int k: {2,3,4,5}) {
        perform_full_check(g, make_distance_oracle_thorup2kminus1approximation(g, k), k*2-1);
    }
}

BOOST_AUTO_TEST_CASE( vv_thorup2kminus1_random_star_test ) {
    for (int s: irange(10)) {
        auto g = SGM::get_star_random(s, 10, 20, 50);
        for (auto k: {2,3,4,5}) {
            perform_full_check(g, make_distance_oracle_thorup2kminus1approximation(g, k), 2*k-1);
        }
    }
}

BOOST_AUTO_TEST_CASE( vv_thorup2kminus1_general_test ) {
    auto g = SGM::get_graph_medium();

    for (int k: {2,3,4,5}) {
        perform_full_check(g, make_distance_oracle_thorup2kminus1approximation(g, k), k*2-1);
    }
}

BOOST_AUTO_TEST_CASE( vv_thorup2kminus1_listgraph_test ) {
    SGM::ListGraph g = SGM::get_list_graph_medium();

    using VD = typename boost::graph_traits<SGM::ListGraph>::vertex_descriptor;
    using Map = std::map<VD, int>;
    using IndexMap = boost::associative_property_map<Map>;

    Map inner_map;
    IndexMap index_map(inner_map);

    for (auto v: indexed_range(boost::make_iterator_range(vertices(g)))) {
        boost::put(index_map, *v, v.index());
    }

    for (int k: {2,3,4,5}) {
        perform_full_check(g, make_distance_oracle_thorup2kminus1approximation(g, k, vertex_index_map(index_map)), k*2-1, index_map);
    }
}

