/**
 * @file 2_local_search_example.cpp
 * @brief 2-opt example.
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */

#include <vector>
#include <iostream>

#include "paal/local_search/2_local_search/2_local_search.hpp"
#include "paal/data_structures/cycle/cycle_algo.hpp"
#include "paal/data_structures/cycle/simple_cycle.hpp"
#include "utils/sample_graph.hpp"

using namespace  paal::local_search::two_local_search;
using namespace  paal;

int main() {
//! [Two Local Search Example]
    //sample data
    typedef  SampleGraphsMetrics SGM;
    auto gm = SGM::getGraphMetricSmall();
    const int size = gm.size();
    std::vector<int> v(size);
    std::iota(v.begin(), v.end(), 0);

    //create random solution 
    std::random_shuffle(v.begin(), v.end());
    typedef data_structures::SimpleCycle<int> Cycle;
    Cycle cycle(v.begin(), v.end());
    std::cout << "Length \t" << simple_algo::getLength(gm, cycle) << std::endl;

    //search
    two_local_search_simple(cycle, getDefaultTwoLocalComponents(gm));

    //printing
    std::cout << "Length \t" << simple_algo::getLength(gm, cycle) << std::endl;

//! [Two Local Search Example]
    return 0;
}

