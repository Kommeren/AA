/**
 * @file knapsack_test.cpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-09-20
 */

#define BOOST_TEST_MODULE knapsack_test
#include <fstream>

#include <boost/test/unit_test.hpp>
#include <boost/range/irange.hpp>

#include "paal/dynamic/knapsack.hpp"
#include "paal/utils/double_rounding.hpp"
#include "utils/logger.hpp"

using namespace paal;


std::vector<int> sizes{1,2,3,1,2,4,1,10};
int capacity = 6;
std::vector<int> values{3,2,65,1,2,3,1,23};
auto objects = boost::irange(size_t(0), sizes.size());


//Knapsack
BOOST_AUTO_TEST_CASE(Knapsack) {
    std::vector<int> result;
    LOG("Knapsack");
    auto maxValue = paal::knapsack(std::begin(objects), std::end(objects), 
            capacity,
            std::back_inserter(result), 
            paal::utils::make_Array2Functor(sizes), 
            paal::utils::make_Array2Functor(values));

    BOOST_CHECK_EQUAL(maxValue.first, 130);
    LOG("Max value " << maxValue.first << ", Total size "  << maxValue.second);
    LOG_COPY_DEL(result.begin(), result.end(), " ");
}

//Knapsack 0/1
BOOST_AUTO_TEST_CASE(Knapsack_0_1) {
    std::vector<int> result;
    LOG("Knapsack");
    auto maxValue = paal::knapsack_0_1(std::begin(objects), std::end(objects), 
            capacity,
            std::back_inserter(result), 
            paal::utils::make_Array2Functor(sizes), 
            paal::utils::make_Array2Functor(values));

    BOOST_CHECK_EQUAL(maxValue.first, 70);
    LOG("Max value " << maxValue.first << ", Total size "  << maxValue.second);
    LOG_COPY_DEL(result.begin(), result.end(), " ");
}
