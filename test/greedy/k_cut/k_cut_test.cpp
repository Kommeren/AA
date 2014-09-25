//=======================================================================
// Copyright (c) 2013 Piotr Smulewicz
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file k_cut_test.cpp
 * @brief
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2013-08-20
 */

#include "utils/logger.hpp"
#include "utils/test_result_check.hpp"

#include "paal/greedy/k_cut/k_cut.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/range/irange.hpp>
#include <boost/test/unit_test.hpp>


template <typename Graph>
std::pair<Graph, std::vector<int>> create_instance() {
    std::vector<std::pair<int,int>> edges_p{{1,2},{1,5},{2,3},{2,5},{2,6},
        {3,4},{3,7},{4,7},{4,0},{5,6},{6,7},{7,0}};
    std::vector<int> costs{2,3,3,2,2,4,2,2,2,3,1,3};

    Graph graph(edges_p.begin(), edges_p.end(), costs.begin(), 8);
    std::vector<int> optimal = {4,9,13,16,21,25,29};

    return std::make_pair(graph, optimal);
}

template <typename Graph>
void run_test(const Graph &graph, const std::vector<int> &optimal) {
    using VT = typename boost::graph_traits<Graph>::vertex_descriptor;
    auto index = get(boost::vertex_index, graph);
    auto weight = get(boost::edge_weight, graph);

    for (auto i: boost::irange(2,9)) {
        std::vector<std::pair<VT, int>> vertices_parts;
        int cost_cut = paal::greedy::k_cut(graph, i, back_inserter(vertices_parts),
            boost::weight_map(weight));
        LOGLN("cost cut: " << cost_cut);
        std::vector<int> vertices_to_parts;
        vertices_to_parts.resize(vertices_parts.size());
        for (auto i: vertices_parts){
            LOG(i.first << "(" << i.second << "), ");
            vertices_to_parts[get(index, i.first)] = i.second;
        }
        LOGLN("");

        int cost_cut_verification = 0;
        for (auto v: boost::make_iterator_range(edges(graph))) {
            if (vertices_to_parts[get(index, source(v, graph))]
                    != vertices_to_parts[get(index, target(v, graph))])
                cost_cut_verification += get(weight, v);
        }
        BOOST_CHECK_EQUAL(cost_cut, cost_cut_verification);
        check_result(cost_cut_verification, optimal[i-2], 2.0 - 2.0 / double(i));
    }
}

template <typename VertexList>
using Graph = boost::adjacency_list<boost::vecS, VertexList, boost::undirectedS,
                                    boost::property<boost::vertex_index_t, int>,
                                    boost::property<boost::edge_weight_t, int>>;

BOOST_AUTO_TEST_CASE(KCut) {
    auto instance = create_instance<Graph<boost::vecS>>();
    run_test(instance.first, instance.second);
}

BOOST_AUTO_TEST_CASE(KCut_list) {
    auto instance = create_instance<Graph<boost::listS>>();
    auto graph = instance.first;
    auto optimal = instance.second;

    auto index = get(boost::vertex_index, graph);
    int idx = 0;
    for (auto v : boost::make_iterator_range(vertices(graph))) {
        put(index, v, idx);
        ++idx;
    }

    run_test(graph, optimal);
}
