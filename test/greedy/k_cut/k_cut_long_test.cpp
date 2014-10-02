//=======================================================================
// Copyright (c) 2013 Piotr Smulewicz
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file k_cut_long_test.cpp
 * @brief
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2013-08-20
 */

#include "utils/logger.hpp"
#include "utils/test_result_check.hpp"

#include "paal/greedy/k_cut/k_cut.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/test/unit_test.hpp>


const int nu_vertices = 500;
const int nu_edges = 1000*100;
const int seed = 43;
const int parts = 20;
const int max_edge_weight_in_components = 10000;
const int max_edge_weight_between_components = 10;
BOOST_AUTO_TEST_CASE(KCut) {
    LOGLN("wertices: " << nu_vertices << " edges: " << nu_edges);
    LOGLN("parts: " << parts);
    //generate graph
    std::vector<std::pair<int, int>> edges_p;
    std::vector<long long> cost_edges;
    std::srand(seed);
    long long cost_cut_oncomponents = 0;
    {
        int source, target, edge_cost, nu_edges_copy = nu_edges;
        while (--nu_edges_copy) {
            source = rand() % nu_vertices;
            target = rand() % nu_vertices;
            edges_p.push_back(std::make_pair(source, target));
            if (source % parts == target % parts) {
                edge_cost = (rand() % max_edge_weight_in_components);
            } else {
                edge_cost = (rand() % max_edge_weight_between_components);
                cost_cut_oncomponents += edge_cost;
            }
            cost_edges.push_back(edge_cost);
        }
    }
    boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS,
                    boost::no_property,
                    boost::property < boost::edge_weight_t, int>
                    > graph(edges_p.begin(), edges_p.end(), cost_edges.begin(), nu_vertices);
    //solve
    std::vector<std::pair<int, int>> vertices_parts;
    long long cost_cut = paal::greedy::k_cut(graph, parts, back_inserter(vertices_parts));
    //print result
    LOGLN("cost cut: " << cost_cut);
    std::vector<int> vertices_to_parts;
    vertices_to_parts.resize(vertices_parts.size());
    for (auto i:vertices_parts) {
        LOG(i.first << "(" << i.second << "), ");
        vertices_to_parts[i.first] = i.second;
    }
    LOGLN("");
    //verificate result
    auto weight = get(boost::edge_weight, graph);
    long long cost_cut_verification = 0;
    auto all_edges = edges(graph);
    for (auto edge : boost::make_iterator_range(all_edges)) {
        if (vertices_to_parts[source(edge, graph)] != vertices_to_parts[target(edge, graph)])
            cost_cut_verification += weight(edge);
    }
    BOOST_CHECK_EQUAL(cost_cut, cost_cut_verification);
	LOGLN("Number of parts: " << parts);
    //estimate aproximation ratio
    check_result_compare_to_bound(cost_cut_verification, cost_cut_oncomponents,
                    2.0 - 2.0 / double(parts), paal::utils::less_equal(),
                    0LL, "cut cost on components: ");
}
