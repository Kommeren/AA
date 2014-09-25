//=======================================================================
// Copyright (c) 2014 Piotr Smulewicz
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file read_orlib_km.hpp
 * @brief
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2014-08-01
 */

/* Data files contains multiple edges
 * only last cost of edges that have same set Vertex) is used
 */
#ifndef READ_ORLIB_KM_HPP
#define READ_ORLIB_KM_HPP

#include <boost/test/unit_test.hpp>

#include "utils/logger.hpp"

#include "paal/data_structures/metric/graph_metrics.hpp"

#include <type_traits>
#include <unordered_map>
namespace paal {

typedef boost::adjacency_list<
    boost::vecS, boost::vecS, boost::undirectedS, boost::no_property,
    boost::property<boost::edge_weight_t, int>> graph_t;
typedef std::pair<int, int> Edge;
typedef paal::data_structures::graph_metric<graph_t, int> GraphMT;

GraphMT read_orlib_KM(std::istream &ist, boost::integer_range<int> &fac,
                      boost::integer_range<int> &clients) {
    int V, nE, K;
    ist >> V >> nE >> K;

    fac = boost::irange(0, K);
    clients = boost::irange(K, V);

    data_structures::array_metric<long long> m(V);

    int a, b;
    int l;
    std::vector<Edge> edges;
    std::vector<int> weight;
    std::unordered_map<std::pair<int, int>, int,
                       boost::hash<std::pair<int, int>>> edges_position;
    int j = 1;
    while (nE--) {
        ist >> a >> b >> l;
        if (a < b) {
            std::swap(a, b);
        }
        BOOST_REQUIRE(edges_position[std::make_pair(a, b)] < j);

        if (edges_position[std::make_pair(a, b)] == 0) {
            edges_position[std::make_pair(a, b)] = j;
            j++;
            edges.push_back(Edge(a - 1, b - 1));
            weight.push_back(l);
        } else {
            weight[edges_position[std::make_pair(a, b)] - 1] = l;
        }
    }
    graph_t g(edges.begin(), edges.end(), weight.begin(), V);
    return data_structures::graph_metric<graph_t, int>(std::move(g));
}
}
#endif /* READ_ORLIB_FC_HPP */
