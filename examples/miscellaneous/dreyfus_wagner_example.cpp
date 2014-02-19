/**
 * @file dreyfus_wagner_example.cpp
 * @brief
 * @author Maciej Andrejczuk
 * @version 1.0
 * @date 2013-08-01
 */
#include <iostream>
#include "paal/steiner_tree/dreyfus_wagner.hpp"
#include "test/utils/sample_graph.hpp"

int main() {
//! [Dreyfus Wagner Example]
    // prepare metric
    typedef  SampleGraphsMetrics SGM;
    auto gm = SGM::getGraphMetricSteiner();

    // prepare terminals and Steiner vertices
    std::vector<int> terminals = {SGM::A, SGM::B, SGM::C, SGM::D};
    std::vector<int> nonterminals = {SGM::E};

    // run algorithm
    auto dw = paal::steiner_tree::make_DreyfusWagner(gm, terminals, nonterminals);
    dw.solve();

    // print result
    std::cout << "Cost = " << dw.getCost() << std::endl;
    std::cout << "Steiner points:" << std::endl;
    std::set<int> steinerPoints = dw.steinerTreeZelikovsky11per6approximation();
    std::copy(steinerPoints.begin(), steinerPoints.end(), std::ostream_iterator<int>(std::cout, "\n"));
    std::cout << "Edges:" << std::endl;
    for (auto edge: dw.getEdges()) {
        std::cout << "(" << edge.first << "," << edge.second << ")" << std::endl;
    }

//! [Dreyfus Wagner Example]
    return 0;
}
