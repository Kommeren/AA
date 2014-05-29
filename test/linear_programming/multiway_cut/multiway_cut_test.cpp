/**
 * @file multiway_cut_test.cpp
 * @brief
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2013-01-09
 */

#include "utils/logger.hpp"
#include "utils/test_result_check.hpp"

#include "paal/multiway_cut/multiway_cut.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/range/irange.hpp>
#include <boost/graph/adjacency_list.hpp>


template <typename Graph>
std::pair<Graph, int> create_instance() {
    std::vector<std::pair<int,int> > edgesP{{0,3},{1,3},
                                            {0,4},{2,4},
                                            {1,5},{2,5},
                                            {3,6},{4,6},
                                            {3,7},{5,7},
                                            {4,8},{5,8},
                                            {6,7},{6,8},{7,8}
    };
    const int nu_vertices = 9;
    std::vector<int> costEdges{100,100,100,100,100,100,10,10,10,10,10,10,1,1,1};

    Graph graph(edgesP.begin(), edgesP.end(), costEdges.begin(), nu_vertices);

    const int terminals_num = 3;
    int i = 0;
    for (auto v : boost::make_iterator_range(vertices(graph))) {
        put(boost::vertex_color, graph, v, i+1);
        ++i;
        if (i == terminals_num) {
            break;
        }
    }

    const int optimal = 320;
    return std::make_pair(graph, optimal);
}

template <typename Graph>
void run_test(const Graph & graph, int optimal) {
    using VT = typename boost::graph_traits<Graph>::vertex_descriptor;
    auto index = get(boost::vertex_index, graph);
    auto weight = boost::get(boost::edge_weight, graph);
    std::vector<std::pair<VT, int>> verticesParts;

    auto costCut = paal::multiway_cut(graph, back_inserter(verticesParts));
    LOGLN("cost cut: " << costCut);
    std::vector<int> verticesToParts(verticesParts.size());
    for (auto i: verticesParts) {
        LOG(i.first << "(" << i.second << "), ");
        verticesToParts[get(index, i.first)] = i.second;
    }
    LOGLN("");
    int costCutVerification = 0;
    for (auto e : boost::make_iterator_range(edges(graph))){
        if (verticesToParts[get(index, source(e, graph))] != verticesToParts[get(index, target(e, graph))])
            costCutVerification += get(weight, e);
    }

    check_result(costCut, optimal, 2);
    LOGLN("Cost Cut Verification: " << costCutVerification);
    BOOST_CHECK_EQUAL(costCut, costCutVerification);
}

template <typename VertexList>
using Graph = boost::adjacency_list<boost::vecS, VertexList, boost::undirectedS,
                boost::property<boost::vertex_index_t, int,
                    boost::property<boost::vertex_color_t, int>>,
                boost::property<boost::edge_weight_t, int>>;

BOOST_AUTO_TEST_CASE(multiway_cutS) {
    auto instance = create_instance<Graph<boost::vecS>>();
    run_test(instance.first, instance.second);
}

BOOST_AUTO_TEST_CASE(multiway_cutS_list) {
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

