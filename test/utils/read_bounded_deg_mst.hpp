/**
 * @file read_bounded_deg_mst.hpp
 * @brief
 * @author Piotr Godlewski
 * @version 1.0
 * @date 2013-06-10
 */
#ifndef READ_BOUNDED_DEG_MST_HPP
#define READ_BOUNDED_DEG_MST_HPP

#include <boost/graph/graph_traits.hpp>

#include <string>
#include <cassert>

namespace paal {

template <typename Graph, typename Cost>
void add_edge_to_graph(Graph & g, Cost & cost, int u, int v, double c) {
    bool b;
    typename boost::graph_traits < Graph >::edge_descriptor e;
    std::tie(e, b) = add_edge(u, v, g);
    assert(b);
    cost[e] = c;
}

template <typename Graph, typename Cost, typename Bounds>
inline void read_bdmst(std::istream & is, int verticesNum, int edgesNum,
                Graph & g, Cost & costs, Bounds & degBounds, double & bestCost) {
    std::string s;
    int u, v, b;
    double c;

    is >> s; is >> s; is >> s;

    for (int i = 0; i < verticesNum; i++) {
        is >> u >> b;
        degBounds[u] = b;
    }

    is >> s; is >> s; is >> s;

    for (int i = 0; i < edgesNum; i++) {
        is >> u >> v >> b >> c;
        add_edge_to_graph(g, costs, u, v, c);
    }

    is >> s; is >> s;
    is >> bestCost;
}

}
#endif /* READ_BOUNDED_DEG_MST_HPP */
