/**
 * @file steiner_tree_greedy_test.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-09-20
 */


#include <boost/test/unit_test.hpp>

#include "paal/greedy/steiner_tree_greedy.hpp"

#include "utils/logger.hpp"
#include "utils/sample_graph.hpp"

BOOST_AUTO_TEST_SUITE(SteinerTreeGreedy)

BOOST_AUTO_TEST_CASE(SteinerTreeTest) {
    typedef  SampleGraphsMetrics SGM;
    auto g = SGM::getGraphSteiner();
    typedef boost::graph_traits<decltype(g)>::edge_descriptor Edge;
    std::set<Edge> steinerEdges;

    paal::steiner_tree_greedy(g, std::inserter(steinerEdges, steinerEdges.begin()));
    BOOST_CHECK(size_t(3) <= steinerEdges.size() && steinerEdges.size()<= size_t(4));
    auto weight = get(boost::edge_weight, g);
    auto sum = 0;
    for(auto e: steinerEdges) {
        sum += get(weight, e);
    }
    BOOST_CHECK(sum <= 2 * 4);

}

BOOST_AUTO_TEST_CASE(SteinerTreeTestParameters) {
    typedef  SampleGraphsMetrics SGM;
    auto g = SGM::getGraphSteiner();
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

    paal::steiner_tree_greedy(g, std::inserter(steinerEdges, steinerEdges.begin()),
            boost::vertex_color_map(&color[0]));
    BOOST_CHECK(size_t(3) <= steinerEdges.size() && steinerEdges.size()<= size_t(4));
    auto weight = get(boost::edge_weight, g);
    auto sum = 0;
    for(auto e: steinerEdges) {
        sum += get(weight, e);
    }
    BOOST_CHECK(sum <= 2 * 4);
}

BOOST_AUTO_TEST_SUITE_END()
