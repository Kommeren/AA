#include "data_structures/graph_metrics.hpp"

#ifndef __SAMPLE_GRAPH_METRICS__
#define __SAMPLE_GRAPH_METRICS__

struct SampleGraphsMetrics {
    typedef boost::adjacency_list <boost::vecS, boost::vecS, boost::undirectedS, boost::no_property, boost::property < boost::edge_weight_t, int > > graph_t;
//    typedef boost::graph_traits < graph_t >::vertex_descriptor vertex_descriptor;
//    typedef boost::graph_traits < graph_t >::edge_descriptor edge_descriptor;
    typedef std::pair<int, int> Edge;
    typedef paal::data_structures::GraphMetric<graph_t, int> GraphMT;
    enum nodes { A, B, C, D, E, F, G, H };

    static GraphMT getGraphMetricSmall() {
        const int num_nodes = 5;
        Edge edge_array[] = { Edge(A, C), Edge(B, B), Edge(B, D), Edge(B, E),
            Edge(C, B), Edge(C, D), Edge(D, E), Edge(E, A), Edge(E, B)
        };
        int weights[] = { 1, 2, 1, 2, 7, 3, 1, 1, 1 };
        int num_arcs = sizeof(edge_array) / sizeof(Edge);

        graph_t g(edge_array, edge_array + num_arcs, weights, num_nodes);

        return GraphMT(g);
    }
    
    static GraphMT getGraphMetricSteiner() {
        const int num_nodes = 5;
        Edge edge_array[] = { Edge(A, B), Edge(B, C), Edge(C, D), Edge(D, A),
                              Edge(A, E), Edge(B, E), Edge(C, E), Edge(D, E)
        };
        int weights[] = { 2, 2, 2, 2, 
                          1, 1, 1, 1 };
        int num_arcs = sizeof(edge_array) / sizeof(Edge);

        graph_t g(edge_array, edge_array + num_arcs, weights, num_nodes);

        return GraphMT(g);
    } 
    
    static GraphMT getGraphMetricMedium() {
        const int num_nodes = 8;
        Edge edge_array[] = { Edge(A, C), Edge(A, F), Edge(B, E), Edge(B, G),
                              Edge(B, H), Edge(C, E), Edge(C, G), Edge(C, H), 
                              Edge(D, E), Edge(D, G), Edge(D, H), Edge(F, H)
        };
        int weights[] = { 4, 2, 1, 2, 
                          7, 3, 1, 8, 
                          1, 3, 4, 10 };
        int num_arcs = sizeof(edge_array) / sizeof(Edge);

        graph_t g(edge_array, edge_array + num_arcs, weights, num_nodes);

        return GraphMT(g);
    } 


};

#endif //__SAMPLE_GRAPH_METRICS__
