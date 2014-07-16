/**
  * @file distance_oracle_example.cpp
  * @brief
  * @author Jakub Ocwieja
  * @version 1.0
  * @date 2014-04-28
  */

//! [Distance Oracle Example]
#include "paal/distance_oracle/vertex_vertex/thorup_2kminus1.hpp"
#include "test/utils/sample_graph.hpp"

#include <iostream>

int main() {
    using SGM = sample_graphs_metrics;
    using Graph = SGM::Graph;

    const Graph g = SGM::get_graph_small();
    const int k = 2;

    auto oracle = paal::make_distance_oracle_thorup2kminus1approximation(g, k);

    std::cout << "Approximate distance between A and C is " << oracle(SGM::A, SGM::C) << std::endl;
}
//! [Distance Oracle Example]
