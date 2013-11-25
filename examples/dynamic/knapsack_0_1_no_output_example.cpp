/**
 * @file knapsack_0_1_no_output_example.cpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-09-20
 */

#include <vector>
#include <boost/range/irange.hpp>
#include "paal/dynamic/knapsack_0_1.hpp"

int main() {
//! [Knapsack Example]

    std::vector<int> sizes{1,2,3,1,2,4,1,10};
    int capacity = 6;
    std::vector<int> values{3,2,65,1,2,3,1,23};
    auto objects = boost::irange(std::size_t(0), sizes.size());

    //Knapsack 0/1 does not compute the items in knapsack
    std::cout << "Knapsack 0 / 1 no output" << std::endl;
    auto maxValue = paal::knapsack_0_1_no_output(std::begin(objects), std::end(objects), 
            capacity,
            paal::utils::make_ArrayToFunctor(sizes), 
            paal::utils::make_ArrayToFunctor(values));

    std::cout << "Max value " << maxValue.first << ", Total size "  << maxValue.second << std::endl;
//! [Knapsack Example]

    return 0;
}


