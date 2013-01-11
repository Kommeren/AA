/*
 *  * main.cpp
 *  *
 *  *  Created on: 03-01-2013
 *  *      Author: Piotr Wygocki
 *  */

#include <iostream>
#include <iterator>

#include "local_search/2_local_search/2_local_search.hpp"
#include "data_structures/graph_metrics.hpp"

std::ostream& operator<< (std::ostream &o, const std::pair<int, int> &p) {
    return o << p.first << ',' << p.second;
}

using namespace paal;
using namespace paal::local_search;
using namespace paal::data_structures;
using namespace boost;

int main() {

    std::vector<int> v = {1,2,3,4,5, 6, 7, 8, 9, 10};
    auto vExl = TrivialNeigbourGetter().getNeighbourhood(v.begin(), v.end(), 4); 
    std::cout << (vExl.first == vExl.second) << std::endl << std::flush;
    auto f = std::function<bool(int)>(std::bind(std::equal_to<int>(), 4, std::placeholders::_1));
    f(2);
    std::cout << "t1" << std::endl << std::flush;
    std::copy(vExl.first, vExl.second, std::ostream_iterator<int>(std::cout, "\n"));

    SimpleCycleManager<int> cm(vExl.first, vExl.second);
    auto r = cm.getEdgeRange(2);
    for(;r.first != r.second; ++r.first) {
        std::cout << *r.first << std::endl;
    }
    std::cout << "xxx" << std::endl;

    typedef std::pair<int, int> PT;
    cm.flip(2, 7);
    
    r = cm.getEdgeRange(2);
    
    
    for(;r.first != r.second; ++r.first) {
        std::cout << *r.first << std::endl;
    }
    std::cout << "xxx" << std::endl;


    typedef adjacency_list <vecS, vecS, undirectedS, no_property, property < edge_weight_t, int > > graph_t;
    typedef graph_traits < graph_t >::vertex_descriptor vertex_descriptor;
    typedef graph_traits < graph_t >::edge_descriptor edge_descriptor;
    typedef std::pair<int, int> Edge;


    const int num_nodes = 5;
    enum nodes { A, B, C, D, E };
    Edge edge_array[] = { Edge(A, C), Edge(B, B), Edge(B, D), Edge(B, E),
               Edge(C, B), Edge(C, D), Edge(D, E), Edge(E, A), Edge(E, B)
    };
    int weights[] = { 1, 2, 1, 2, 7, 3, 1, 1, 1 };
    int num_arcs = sizeof(edge_array) / sizeof(Edge);
    
    graph_t g(edge_array, edge_array + num_arcs, weights, num_nodes);
    property_map<graph_t, edge_weight_t>::type weightmap = get(edge_weight, g);

    GraphMetric<graph_t, int> gm(g,weightmap);

    std::cout << "odl: " << gm(A,B) << std::endl;
    std::cout << "odl: " << gm(C,B) << std::endl;




}
