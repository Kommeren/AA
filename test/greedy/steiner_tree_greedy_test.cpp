/**
 * @file steiner_tree_greedy_test.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-09-20
 */

#include "utils/logger.hpp"
#include "utils/sample_graph.hpp"
#include "utils/test_result_check.hpp"

#include "paal/greedy/steiner_tree_greedy.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/property_map/function_property_map.hpp>

#include <vector>
#include <map>

BOOST_AUTO_TEST_SUITE(steiner_tree_greedy)
typedef sample_graphs_metrics SGM;

template <typename Graph, typename Tree> int get_sum(Graph g, Tree &edges) {
    auto weight = get(boost::edge_weight, g);
    auto sum = 0;
    for (auto e : edges) {
        sum += get(weight, e);
    }
    return sum;
}

template <typename Graph> int test(Graph g) {
    typedef typename boost::graph_traits<decltype(g)>::edge_descriptor Edge;
    std::vector<Edge> steiner_edges;
    auto result = paal::steiner_tree_greedy(
        g, std::inserter(steiner_edges, steiner_edges.begin()));
    auto result_check = get_sum(g, steiner_edges);
    BOOST_CHECK_EQUAL(result_check,result.first);
    return result.first;
}
BOOST_AUTO_TEST_CASE(steiner_tree_test) {
    const int OPTIMAL = 4;
    const int APPROXIMATION_RATIO = 2;
    LOGLN("steiner_tree_multi_edges");
    check_result(test(SGM::get_graph_steiner()), OPTIMAL, APPROXIMATION_RATIO);
}

BOOST_AUTO_TEST_CASE(not_conected_terminals) {
    LOGLN("not conected teerminals");
    BOOST_CHECK_EQUAL(test(SGM::two_points_steiner()), 0);
}

BOOST_AUTO_TEST_CASE(steiner_tree_multi_edges_test) {
    LOGLN("steiner_tree_multi_edges");
    BOOST_CHECK_EQUAL(test(SGM::get_graph_steiner_multi_edges()), 23);
}

BOOST_AUTO_TEST_CASE(steiner_tree_test_cycle) {
    LOGLN("steiner_tree_test_cycle");
    BOOST_CHECK_EQUAL(test(SGM::get_graph_stainer_tree_cycle()), 10);
}

BOOST_AUTO_TEST_CASE(steiner_tree_test_no_weight_in_graph) {
    LOGLN("no weights in graph");
    typedef sample_graphs_metrics SGM;
    auto g = SGM::get_graph_steiner_edge();
    typedef boost::graph_traits<decltype(g)>::edge_descriptor Edge;
    std::vector<Edge> steiner_edges;
    auto color = get(boost::vertex_color, g);
    std::map<Edge, int> edge_weight;
    boost::associative_property_map<std::map<Edge, int>>
    edge_weight_property_map(edge_weight);
    for (auto w : boost::make_iterator_range(edges(g))) {
        edge_weight_property_map[w] = 1;
    }
    paal::steiner_tree_greedy(
        g, std::inserter(steiner_edges, steiner_edges.begin()),
        edge_weight_property_map, color);
}

BOOST_AUTO_TEST_CASE(steiner_tree_test_Parameters) {
    const int OPTIMAL = 4;
    const int APPROXIMATION_RATIO = 2;
    LOGLN("steiner_tree_test_Parameters");
    typedef sample_graphs_metrics SGM;
    auto g = SGM::get_graph_steiner();
    typedef typename boost::graph_traits<decltype(g)>::edge_descriptor Edge;
    std::vector<Edge> steiner_edges;
    std::vector<int> color(num_vertices(g));
    {
        auto c = &color[0];
        put(c, SGM::A, paal::Terminals::TERMINAL);
        put(c, SGM::B, paal::Terminals::TERMINAL);
        put(c, SGM::C, paal::Terminals::TERMINAL);
        put(c, SGM::D, paal::Terminals::TERMINAL);
        put(c, SGM::E, paal::Terminals::NONTERMINAL);
    }
    auto index = get(boost::vertex_index, g);
    auto colorPMap = boost::make_iterator_property_map(color.begin(), index);

    paal::steiner_tree_greedy(
        g, std::inserter(steiner_edges, steiner_edges.begin()),
        boost::vertex_color_map(colorPMap));
    BOOST_CHECK(std::size_t(3) <= steiner_edges.size() &&
                steiner_edges.size() <= std::size_t(4));
    check_result(get_sum(g, steiner_edges), OPTIMAL, APPROXIMATION_RATIO);
}

BOOST_AUTO_TEST_SUITE_END()
