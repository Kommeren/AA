/**
 * @file read_bounded_deg_mst.hpp
 * @brief 
 * @author Piotr Godlewski
 * @version 1.0
 * @date 2013-06-10
 */
#ifndef READ_BOUNDED_DEG_MST_HPP
#define READ_BOUNDED_DEG_MST_HPP 

#include <string>

namespace paal {

using namespace boost;

typedef adjacency_list < vecS, vecS, undirectedS,
                        property < vertex_degree_t, int,
                            property < vertex_index_t, int >
                                 >,
                        property < edge_weight_t, double > > Graph;
typedef adjacency_list_traits < vecS, vecS, undirectedS > Traits;
typedef graph_traits < Graph >::edge_descriptor Edge;

typedef property_map < Graph, vertex_degree_t >::type Bound;
typedef property_map < Graph, vertex_index_t >::type Index;
typedef property_map < Graph, edge_weight_t >::type Cost;

template <typename Graph, typename Cost>
Edge addEdge(Graph & g, Cost & cost, int u, int v, double c) {
    bool b;
    Traits::edge_descriptor e;
    std::tie(e, b) = add_edge(u, v, g);
    assert(b);
    cost[e] = c;
    return e;
}
  
void readBDMST(std::istream & is, int verticesNum, int edgesNum,
               Graph & g, Cost & costs, Bound & degBounds, Index & indices,
               double & bestCost) {
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
        addEdge(g, costs, u, v, c);
    }
    
    is >> s; is >> s;
    is >> bestCost;
}

}
#endif /* READ_BOUNDED_DEG_MST_HPP */
