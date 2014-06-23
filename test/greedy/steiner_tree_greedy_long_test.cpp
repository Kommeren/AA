/**
 * @file steiner_tree_greedy_long_test.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-09-20
 */

#include "utils/read_steinlib.hpp"
#include "utils/test_result_check.hpp"

#include "paal/greedy/steiner_tree_greedy.hpp"
#include "paal/utils/functors.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/range/join.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/prim_minimum_spanning_tree.hpp>
#include <boost/graph/connected_components.hpp>

template <typename Edge> struct edge_filter {
    bool operator()(Edge e) const { return edges.find(e) != edges.end(); }
    std::set<Edge> edges;
};

BOOST_AUTO_TEST_CASE(steiner_tree_greedy_test) {

    std::vector<paal::steiner_tree_test> data;
    LOGLN("READING INPUT...");
    read_steinlib_tests(data);
    for (paal::steiner_tree_test &test : data) {
        LOGLN("TEST " << test.test_name);
        LOGLN("OPT " << test.optimal);

        auto const &g = test.graph;
        auto N = num_vertices(g);
        typedef puretype(g) Graph;
        typedef boost::graph_traits<Graph>::edge_descriptor Edge;
        edge_filter<Edge> eFilter;

        paal::steiner_tree_greedy(
            g, std::inserter(eFilter.edges, eFilter.edges.begin()));

        LOGLN("terminals:");
        LOG_COPY_RANGE_DEL(test.terminals, ",");
        LOGLN("");

        LOGLN("chosen edges:");
        LOG_COPY_RANGE_DEL(eFilter.edges, "'");
        LOGLN("");
        boost::filtered_graph<Graph, edge_filter<Edge>> fg(g, eFilter);

        std::vector<int> components(N);
        std::vector<int> colorMap(N);
        auto index = get(boost::vertex_index, g);
        auto colorPMap =
            boost::make_iterator_property_map(colorMap.begin(), index);
        boost::connected_components(fg, &components[0],
                                    boost::color_map(colorPMap));
        auto treeColor = components[test.terminals.front()];

        for (auto v : boost::make_iterator_range(vertices(fg))) {
            if (std::find(test.terminals.begin(), test.terminals.end(), v) !=
                test.terminals.end()) {
                BOOST_CHECK_EQUAL(components[v], treeColor);
            }
        }

        auto weight = get(boost::edge_weight, g);
        int res(0);
        for (auto e : eFilter.edges) {
            res += weight(e);
        }
        check_result(res, test.optimal, 2.);
    }
}
