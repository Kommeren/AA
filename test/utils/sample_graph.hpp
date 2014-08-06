/**
 * @file sample_graph.hpp
 * @brief
 * @author Piotr Wygocki, Piotr Godlewski
 * @version 1.0
 * @date 2013-08-04
 */
#ifndef SAMPLE_GRAPH_HPP
#define SAMPLE_GRAPH_HPP

#include "paal/data_structures/metric/graph_metrics.hpp"
#include "paal/data_structures/metric/euclidean_metric.hpp"

#include <boost/graph/adjacency_list.hpp>

struct sample_graphs_metrics {
    using EdgeProp = boost::property<boost::edge_weight_t, int>;
    using Graph = boost::adjacency_list<
        boost::vecS, boost::vecS, boost::undirectedS,
        boost::property<boost::vertex_color_t, int>, EdgeProp>;
    using GraphWithoutEdgeWeight = boost::adjacency_list<
        boost::vecS, boost::vecS, boost::undirectedS,
        boost::property<boost::vertex_color_t, int>, boost::no_property>;
    using Edge = std::pair<int, int>;
    using GraphMT = paal::data_structures::graph_metric<Graph, int>;
    using Terminals = std::vector<int>;

    enum nodes { A, B, C, D, E, F, G, H };

    // graph small
    static Graph get_graph_small() {
        const int num_nodes = 5;
        Edge edge_array[] = { Edge(A, C), Edge(B, B), Edge(B, D),
                              Edge(B, E), Edge(C, B), Edge(C, D),
                              Edge(D, E), Edge(E, A), Edge(E, B) };
        int weights[] = { 1, 2, 1, 2, 7, 3, 1, 1, 1 };
        int num_arcs = sizeof(edge_array) / sizeof(Edge);

        Graph g(edge_array, edge_array + num_arcs, weights, num_nodes);

        return g;
    }

    static GraphMT get_graph_metric_small() {
        return GraphMT(get_graph_small());
    }

    // graph steiner
    static Graph get_graph_steiner() {
        const int num_nodes = 6;
        Edge edge_array[] = { Edge(A, B), Edge(B, C), Edge(C, D),
                              Edge(D, A), Edge(A, E), Edge(B, E),
                              Edge(C, E), Edge(D, E), Edge(A, F) };
        int weights[] = { 2, 2, 2, 2, 1, 1, 1, 1, 1 };
        int num_arcs = sizeof(edge_array) / sizeof(Edge);

        Graph g(edge_array, edge_array + num_arcs, weights, num_nodes);
        auto color = get(boost::vertex_color, g);
        put(color, A, 1);
        put(color, B, 1);
        put(color, C, 1);
        put(color, D, 1);

        return g;
    }

    static Graph two_points_steiner() {
        const int num_nodes = 2;
        Edge edge_array[] = {};
        int weights[] = {};
        Graph g(edge_array, edge_array, weights, num_nodes);
        auto color = get(boost::vertex_color, g);
        put(color, A, 1);
        put(color, B, 1);
        return g;
    }

    static Graph get_graph_steiner_multi_edges() {
        const int num_nodes = 3;
        Edge edge_array[] = { Edge(A, B), Edge(A, C), Edge(B, C), Edge(B, C),
                              Edge(B, C) };
        int weights[] = { 20, 30, 3, 5, 9 };
        int num_arcs = sizeof(edge_array) / sizeof(Edge);

        Graph g(edge_array, edge_array + num_arcs, weights, num_nodes);
        auto color = get(boost::vertex_color, g);
        put(color, A, 1);
        put(color, B, 1);
        put(color, C, 1);

        return g;
    }
    static GraphWithoutEdgeWeight get_graph_steiner_edge() {

        const int num_nodes = 2;
        Edge edge_array[] = { Edge(A, B) };
        int num_arcs = sizeof(edge_array) / sizeof(Edge);

        GraphWithoutEdgeWeight g(edge_array, edge_array + num_arcs, num_nodes);
        auto color = get(boost::vertex_color, g);
        put(color, A, 1);
        put(color, B, 1);

        return g;
    }

    static Graph get_graph_stainer_tree_cycle() {
        const int num_nodes = 5;
        Edge edge_array[] = { Edge(A, B), Edge(A, C), Edge(B, C), Edge(C, D),
                              Edge(C, E) };
        int weights[] = { 4, 2, 2, 2, 2 };
        int num_arcs = sizeof(edge_array) / sizeof(Edge);

        Graph g(edge_array, edge_array + num_arcs, weights, num_nodes);
        auto color = get(boost::vertex_color, g);
        put(color, A, 1);
        put(color, B, 1);
        put(color, D, 1);
        put(color, E, 1);

        return g;
    }

    static GraphMT get_graph_metric_steiner() {
        return GraphMT(get_graph_steiner());
    }

    static std::pair<Terminals, Terminals> get_graph_steiner_vertices() {
        Terminals terminals = { A, B, C, D }, non_terminals = { E, F };
        return std::make_pair(terminals, non_terminals);
    }

    // graph medium
    static GraphMT get_graph_metric_medium() {
        const int num_nodes = 8;
        Edge edge_array[] = { Edge(A, C), Edge(A, F), Edge(B, E), Edge(B, G),
                              Edge(B, H), Edge(C, E), Edge(C, G), Edge(C, H),
                              Edge(D, E), Edge(D, G), Edge(D, H), Edge(F, H) };
        int weights[] = { 4, 2, 1, 2, 7, 3, 1, 8, 1, 3, 4, 10 };
        int num_arcs = sizeof(edge_array) / sizeof(Edge);

        Graph g(edge_array, edge_array + num_arcs, weights, num_nodes);

        return GraphMT(g);
    }

    // graph steiner bigger
    static Graph get_graph_steiner_bigger(int p = 3, int q = 2) {
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

    static GraphMT get_graph_metric_steiner_bigger() {
        return GraphMT(get_graph_steiner_bigger());
    }

    static std::pair<Terminals, Terminals>
    get_graph_steiner_bigger_vertices(int p = 3, int q = 2) {
        int n = p + p * q;
        Terminals terminals, non_terminals;
        for (int i = 0; i < n; i++) {
            if (i >= p)
                terminals.push_back(i);
            else
                non_terminals.push_back(i);
        }
        return make_pair(terminals, non_terminals);
    }

    // eucildean steiner
    template <typename Points = std::vector<std::pair<int, int>>>
    static std::tuple<paal::data_structures::euclidean_metric<int>, Points,
                      Points>
    get_euclidean_steiner_sample() {
        return std::make_tuple(paal::data_structures::euclidean_metric<int>{},
                               Points{ { 0, 0 }, { 0, 2 }, { 2, 0 }, { 2, 2 } },
                               Points{ { 1, 1 } });
    }
};

#endif // SAMPLE_GRAPH_HPP
