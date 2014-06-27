/**
 * @file knapsack_0_1_on_value_fptas_example.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-09-20
 */

//! [Knapsack Example]
#include "paal/dynamic/knapsack_0_1_fptas.hpp"

#include <vector>
#include <iostream>

int main() {
    using Objects = std::vector<std::pair<int, int>>;
    Objects objects{ { 1, 3 }, { 2, 2 }, { 3, 65 }, { 1, 1 }, { 2, 2 },
                     { 4, 3 }, { 1, 1 }, { 10, 23 } };
    const int capacity = 6;
    auto size = [](std::pair<int, int> object) { return object.first; }
    ;
    auto value = [](std::pair<int, int> object) { return object.second; }
    ;

    // Knapsack
    Objects result;
    std::cout << "Knapsack unbounded FPTAS on value" << std::endl;

    double epsilon = 1. / 4.;
    auto maxValue = paal::knapsack_0_1_on_value_fptas(
        epsilon, objects, capacity, std::back_inserter(result), size, value);

    std::cout << "Max value " << maxValue.first << ", Total size "
              << maxValue.second << std::endl;
    for (auto o : result) {
        std::cout << "{ size = " << o.first << ", value = " << o.second << "} ";
    }
    std::cout << std::endl;


    return 0;
}
//! [Knapsack Example]
