#ifndef SAMPLE_GRAPH_HPP
#define SAMPLE_GRAPH_HPP

#include "paal/data_structures/metric/graph_metrics.hpp"

struct sample_graphs_metrics {
    typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS,
                                  boost::property<boost::vertex_color_t, int>,
                                  boost::property<boost::edge_weight_t, int>>
        graph_t;
    typedef std::pair<int, int> Edge;
    typedef paal::data_structures::graph_metric<graph_t, int> GraphMT;
    enum nodes {
        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H
    };

    static graph_t get_graph_small() {
        const int num_nodes = 5;
        Edge edge_array[] = { Edge(A, C), Edge(B, B), Edge(B, D), Edge(B, E),
                              Edge(C, B), Edge(C, D), Edge(D, E), Edge(E, A),
                              Edge(E, B) };
        int weights[] = { 1, 2, 1, 2, 7, 3, 1, 1, 1 };
        int num_arcs = sizeof(edge_array) / sizeof(Edge);

        graph_t g(edge_array, edge_array + num_arcs, weights, num_nodes);

        return g;
    }

    static GraphMT get_graph_metric_small() {
        return GraphMT(get_graph_small());
    }

    static graph_t get_graph_steiner() {
        const int num_nodes = 5;
        Edge edge_array[] = { Edge(A, B), Edge(B, C), Edge(C, D), Edge(D, A),
                              Edge(A, E), Edge(B, E), Edge(C, E), Edge(D, E) };
        int weights[] = { 2, 2, 2, 2, 1, 1, 1, 1 };
        int num_arcs = sizeof(edge_array) / sizeof(Edge);

        graph_t g(edge_array, edge_array + num_arcs, weights, num_nodes);
        auto color = get(boost::vertex_color, g);
        put(color, A, 1);
        put(color, B, 1);
        put(color, C, 1);
        put(color, D, 1);

        return g;
    }

    static GraphMT get_graph_metric_steiner() {
        return GraphMT(get_graph_steiner());
    }

    static GraphMT get_graph_metric_medium() {
        const int num_nodes = 8;
        Edge edge_array[] = { Edge(A, C), Edge(A, F), Edge(B, E), Edge(B, G),
                              Edge(B, H), Edge(C, E), Edge(C, G), Edge(C, H),
                              Edge(D, E), Edge(D, G), Edge(D, H), Edge(F, H) };
        int weights[] = { 4, 2, 1, 2, 7, 3, 1, 8, 1, 3, 4, 10 };
        int num_arcs = sizeof(edge_array) / sizeof(Edge);

        graph_t g(edge_array, edge_array + num_arcs, weights, num_nodes);

        return GraphMT(g);
    }
};

#endif // SAMPLE_GRAPH_HPP
