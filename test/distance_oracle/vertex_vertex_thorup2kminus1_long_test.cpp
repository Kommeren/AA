//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
* @file vertex_vertex_thorup2kminus1_long_test.cpp
* @brief
* @author Jakub Oćwieja
* @version 1.0
* @date 2014-05-19
*/

#include "paal/distance_oracle/vertex_vertex/thorup_2kminus1.hpp"

#include "test_utils/logger.hpp"
#include "test_utils/read_dist.hpp"
#include "test_utils/parse_file.hpp"
#include "test_utils/test_result_check.hpp"

#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/range/adaptor/sliced.hpp>

#include <boost/test/unit_test.hpp>

#include <fstream>

using namespace paal;

static const int CHECKED_VERTICES_NUM = 50;

template <typename Oracle>
void perform_full_check(const Graph& g, const Oracle& oracle, double ratio) {

    using DistanceType = boost::property_map<Graph, boost::edge_weight_t>::value_type;

    for (auto v: boost::make_iterator_range(vertices(g)) | boost::adaptors::sliced(0, CHECKED_VERTICES_NUM)) {
        std::vector<DistanceType> distances(num_vertices(g));
        // TODO Replace with graph_metric with graph_type::Large specialization when available
        dijkstra_shortest_paths(g, v, boost::distance_map(&distances[0]));

        for (auto u: boost::make_iterator_range(vertices(g))) {
            check_result(oracle(u,v), distances[u], ratio);
        }
    }

}

BOOST_AUTO_TEST_CASE( vv_thorup2kminus1_long_test ) {
    std::string testDir = "test/data/DISTANCE/";

    parse(testDir + "cases.txt", [&](const std::string  & fname, std::istream & is_test_cases) {
        LOGLN("TEST " << fname);

        std::ifstream ifs(testDir + "cases/" + fname + ".in");
        Graph g = read_dist(ifs);

        for (int k: {2,3}) {
            LOGLN("k: " << k);
            perform_full_check(g, make_distance_oracle_thorup2kminus1approximation(g, k), k*2-1);
        }
    });
}
