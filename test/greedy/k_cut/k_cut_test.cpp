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

#include <boost/test/unit_test.hpp>
#include <boost/range/irange.hpp>
#include <boost/graph/adjacency_list.hpp>


template <typename Graph>
std::pair<Graph, std::vector<int>> create_instance() {
    std::vector<std::pair<int,int>> edgesP{{1,2},{1,5},{2,3},{2,5},{2,6},{3,4},{3,7},{4,7},{4,0},{5,6},{6,7},{7,0}};
    std::vector<int> costs{2,3,3,2,2,4,2,2,2,3,1,3};

    Graph graph(edgesP.begin(), edgesP.end(), costs.begin(), 8);
    std::vector<int> optimal = {4,9,13,16,21,25,29};

    return std::make_pair(graph, optimal);
}

template <typename Graph>
void run_test(const Graph & graph, const std::vector<int> & optimal) {
    using VT = typename boost::graph_traits<Graph>::vertex_descriptor;
    auto index = get(boost::vertex_index, graph);
    auto weight = get(boost::edge_weight, graph);

    for (auto i: boost::irange(2,9)) {
        std::vector<std::pair<VT, int>> verticesParts;
        int costCut = paal::greedy::kCut(graph, i, back_inserter(verticesParts), boost::weight_map(weight));
        LOGLN("cost cut: " << costCut);
        std::vector<int> verticesToParts;
        verticesToParts.resize(verticesParts.size());
        for (auto i: verticesParts){
            LOG(i.first << "(" << i.second << "), ");
            verticesToParts[get(index, i.first)] = i.second;
        }
        LOGLN("");

        int costCutVerification = 0;
        for (auto v: boost::make_iterator_range(edges(graph))) {
            if (verticesToParts[get(index, source(v, graph))] != verticesToParts[get(index, target(v, graph))])
                costCutVerification += get(weight, v);
        }
        BOOST_CHECK_EQUAL(costCut, costCutVerification);
        check_result(costCutVerification, optimal[i-2], 2.0 - 2.0 / double(i));
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

