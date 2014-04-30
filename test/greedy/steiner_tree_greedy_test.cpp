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


BOOST_AUTO_TEST_SUITE(steiner_treeGreedy)
static const int OPTIMAL=4;
static const int APPROXIMATION_RATIO =2;
BOOST_AUTO_TEST_CASE(steiner_tree_test) {
    typedef  sample_graphs_metrics SGM;
    auto g = SGM::get_graph_steiner();
    typedef boost::graph_traits<decltype(g)>::edge_descriptor Edge;
    std::set<Edge> steinerEdges;

    paal::steiner_tree_greedy(g, std::inserter(steinerEdges, steinerEdges.begin()));
    BOOST_CHECK(std::size_t(3) <= steinerEdges.size() && steinerEdges.size()<= std::size_t(4));
    auto weight = get(boost::edge_weight, g);
    auto sum = 0;
    for(auto e: steinerEdges) {
        sum += get(weight, e);
    }
    check_result(sum,OPTIMAL,APPROXIMATION_RATIO);
}

BOOST_AUTO_TEST_CASE(steiner_tree_testParameters) {
    typedef  sample_graphs_metrics SGM;
    auto g = SGM::get_graph_steiner();
    typedef typename boost::graph_traits<decltype(g)>::edge_descriptor Edge;
    std::set<Edge> steinerEdges;
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

    paal::steiner_tree_greedy(g, std::inserter(steinerEdges, steinerEdges.begin()),
            boost::vertex_color_map(colorPMap));
    BOOST_CHECK(std::size_t(3) <= steinerEdges.size() && steinerEdges.size()<= std::size_t(4));
    auto weight = get(boost::edge_weight, g);
    auto sum = 0;
    for(auto e: steinerEdges) {
        sum += get(weight, e);
    }
    check_result(sum,OPTIMAL,APPROXIMATION_RATIO);
}

BOOST_AUTO_TEST_SUITE_END()
