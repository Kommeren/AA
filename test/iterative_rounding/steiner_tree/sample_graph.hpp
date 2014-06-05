/**
 * @file sample_graph.hpp
 * @brief
 * @author Maciej Andrejczuk
 * @version 1.0
 * @date 2013-08-04
 */



#include <boost/graph/adjacency_list.hpp>


using EdgeProp = boost::property<boost::edge_weight_t, int>;
using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, boost::no_property, EdgeProp>;
using Edge = boost::graph_traits<Graph>::edge_descriptor;
using Terminals = std::vector<int>;

Graph create_small_graph() {
    Graph g(6);
    bool b;
    b = add_edge(0, 1, EdgeProp(2), g).second;
    assert(b);
    b = add_edge(1, 2, EdgeProp(2), g).second;
    assert(b);
    b = add_edge(2, 3, EdgeProp(2), g).second;
    assert(b);
    b = add_edge(3, 0, EdgeProp(2), g).second;
    assert(b);
    b = add_edge(0, 4, EdgeProp(1), g).second;
    assert(b);
    b = add_edge(1, 4, EdgeProp(1), g).second;
    assert(b);
    b = add_edge(2, 4, EdgeProp(1), g).second;
    assert(b);
    b = add_edge(3, 4, EdgeProp(1), g).second;
    assert(b);
    b = add_edge(0, 5, EdgeProp(1), g).second;
    assert(b);

    return g;
}

std::pair<Terminals, Terminals> get_small_graph_vertices() {
    Terminals terminals, non_terminals;
    for (int i = 0; i < 6; i++) {
        if (i < 4) terminals.push_back(i);
        else non_terminals.push_back(i);
    }
    return make_pair(terminals, non_terminals);
}

Graph create_bigger_graph(int p = 3, int q = 2) {
    bool b;
    int n = p + p * q;
    Graph g(n);
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            int cost = 3;
            if (i < p && j < p) {
                cost = 1;
            } else if ((j - p) / q == i) {
                cost = 2;
            }
            b = add_edge(i, j, EdgeProp(cost), g).second;
            assert(b);
        }
    }
    return g;
}

std::pair<Terminals, Terminals> get_bigger_graph_vertices(int p = 3, int q = 2) {
    int n = p + p * q;
    Terminals terminals, non_terminals;
    for (int i = 0; i < n; i++) {
        if (i >= p) terminals.push_back(i);
        else non_terminals.push_back(i);
    }
    return make_pair(terminals, non_terminals);
}
