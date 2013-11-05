/**
 * @file knapsack_0_1_on_size_fptas_example.cpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-09-20
 */
#ifndef KNAPSACK_CPP
#define KNAPSACK_CPP 

#include <vector>
#include <boost/range/irange.hpp>
#include "paal/dynamic/knapsack_0_1_fptas.hpp"

int main() {
//! [Knapsack Example]

    std::vector<int> sizes{1,2,3,1,2,4,1,10};
    int capacity = 6;
    std::vector<int> values{3,2,65,1,2,3,1,23};
    auto objects = boost::irange(size_t(0), sizes.size());

    //Knapsack
    std::cout << "Knapsack FPTAS on size no output" << std::endl;

    double epsilon = 1./4.;
    auto maxValue = paal::knapsack_0_1_no_output_on_size_fptas(epsilon, std::begin(objects), std::end(objects), 
               capacity,
               paal::utils::make_ArrayToFunctor(sizes), 
               paal::utils::make_ArrayToFunctor(values));

    std::cout << "Max value " << maxValue.first << ", Total size "  << maxValue.second << std::endl;

//! [Knapsack Example]

    return 0;
}


#endif /* KNAPSACK_CPP */
