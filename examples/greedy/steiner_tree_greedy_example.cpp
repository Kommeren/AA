/**
 * @file steiner_tree_greedy_example.cpp
 * @brief This is example for greedy solution for steiner tree problem.
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-09-20
 */

#include <iostream>

#include "paal/greedy/steiner_tree_greedy.hpp"

#include "test/utils/sample_graph.hpp"

int main() {
//! [steiner tree greedy Example]
    typedef  sample_graphs_metrics SGM;
    auto g = SGM::get_graph_steiner();
    auto index = get(boost::vertex_index, g);
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
            boost::vertex_color_map(boost::make_iterator_property_map(color.begin(), index)));
    auto weight = get(boost::edge_weight, g);
    auto sum = 0;
    for(auto e: steinerEdges) {
        sum += get(weight, e);
    }
    std::cout << "result " << sum << std::endl;
//! [steiner tree greedy Example]
}

