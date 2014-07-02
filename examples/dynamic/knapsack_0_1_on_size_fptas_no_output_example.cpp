/**
 * @file knapsack_0_1_on_size_fptas_no_output_example.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-09-20
 */

//! [Knapsack Example]
#include "paal/dynamic/knapsack_0_1_fptas.hpp"

#include <boost/range/irange.hpp>

#include <vector>
#include <iostream>

int main() {

    std::vector<int> sizes{ 1, 2, 3, 1, 2, 4, 1, 10 };
    int capacity = 6;
    std::vector<int> values{ 3, 2, 65, 1, 2, 3, 1, 23 };
    auto objects = boost::irange(std::size_t(0), sizes.size());

    // Knapsack
    std::cout << "Knapsack FPTAS on size no output" << std::endl;

    double epsilon = 1. / 4.;
    auto maxValue = paal::knapsack_0_1_no_output_on_size_fptas(
        epsilon, std::begin(objects), std::end(objects), capacity,
        paal::utils::make_array_to_functor(sizes),
        paal::utils::make_array_to_functor(values));

    std::cout << "Max value " << maxValue.first << ", Total size "
              << maxValue.second << std::endl;


    return 0;
}
//! [Knapsack Example]
