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
#include "paal/dynamic/knapsack_0_1.hpp"
#include "paal/utils/double_rounding.hpp"
#include "utils/logger.hpp"

using namespace paal;


std::vector<int> sizes{1,2,3,1,2,4,1,10};
int capacity = 6;
std::vector<int> values{3,2,65,1,2,3,1,23};
auto objects = boost::irange(size_t(0), sizes.size());
auto sizesFunctor = paal::utils::make_Array2Functor(sizes);
auto valuesFunctor = paal::utils::make_Array2Functor(values);
    
template <typename MaxValue> 
void check(MaxValue maxValue, const std::vector<int> & result) {
    BOOST_CHECK_EQUAL(maxValue.first, 130);
    BOOST_CHECK_EQUAL(maxValue.second, 6);
    LOG("Max value " << maxValue.first << ", Total size "  << maxValue.second);
    LOG_COPY_DEL(result.begin(), result.end(), " ");
    LOG("");
}

template <typename MaxValue> 
void check_0_1_no_output(MaxValue maxValue, const std::vector<int> & result) {
    BOOST_CHECK_EQUAL(maxValue.first, 70);
    BOOST_CHECK_EQUAL(maxValue.second, 6);
    LOG("Max value " << maxValue.first << ", Total size "  << maxValue.second);
    LOG("");
}

template <typename MaxValue> 
void check_0_1(MaxValue maxValue, const std::vector<int> & result) {
    BOOST_CHECK_EQUAL(maxValue.first, 70);
    BOOST_CHECK_EQUAL(maxValue.second, 6);
    LOG("Max value " << maxValue.first << ", Total size "  << maxValue.second);
    LOG_COPY_DEL(result.begin(), result.end(), " ");
    LOG("");
}

//Knapsack 
BOOST_AUTO_TEST_CASE(Knapsack) {
    std::vector<int> result;
    LOG("Knapsack");
    auto maxValue = paal::knapsack(std::begin(objects), std::end(objects), 
            capacity,
            std::back_inserter(result), 
            sizesFunctor, 
            valuesFunctor);

    check(maxValue, result);
}

//Knapsack on size
BOOST_AUTO_TEST_CASE(KnapsackSize) {
    std::vector<int> result;
    LOG("Knapsack Size");
    auto maxValue = paal::knapsack_on_size(std::begin(objects), std::end(objects), 
            capacity,
            std::back_inserter(result), 
            sizesFunctor, 
            valuesFunctor);

    check(maxValue, result);
}

//Knapsack on value
BOOST_AUTO_TEST_CASE(KnapsackValue) {
    std::vector<int> result;
    LOG("Knapsack Value");
    auto maxValue = paal::knapsack_on_value(std::begin(objects), std::end(objects), 
            capacity,
            std::back_inserter(result), 
            sizesFunctor, 
            valuesFunctor);

    check(maxValue, result);
}

//Knapsack 0/1
BOOST_AUTO_TEST_CASE(Knapsack_0_1) {
    std::vector<int> result;
    LOG("Knapsack 0/1");
    auto maxValue = paal::knapsack_0_1(std::begin(objects), std::end(objects), 
            capacity,
            std::back_inserter(result), 
            sizesFunctor, 
            valuesFunctor);

    check_0_1(maxValue, result);
}

//Knapsack 0/1 value
BOOST_AUTO_TEST_CASE(Knapsack_0_1_value) {
    std::vector<int> result;
    LOG("Knapsack 0/1 on value");
    auto maxValue = paal::knapsack_0_1_value(std::begin(objects), std::end(objects), 
            capacity,
            std::back_inserter(result), 
            sizesFunctor, 
            valuesFunctor);

    check_0_1(maxValue, result);
}

//Knapsack 0/1  size
BOOST_AUTO_TEST_CASE(Knapsack_0_1_size) {
    std::vector<int> result;
    LOG("Knapsack 0/1 on size");
    auto maxValue = paal::knapsack_0_1_size(std::begin(objects), std::end(objects), 
            capacity,
            std::back_inserter(result), 
            sizesFunctor, 
            valuesFunctor);

    check_0_1(maxValue, result);
}


//Knapsack 0/1: no output iterator
BOOST_AUTO_TEST_CASE(Knapsack_0_1_no_output) {
    std::vector<int> result;
    LOG("Knapsack 0/1 no output");
    auto maxValue = paal::knapsack_0_1_no_output(std::begin(objects), std::end(objects), 
            capacity,
            sizesFunctor, 
            valuesFunctor);

    check_0_1_no_output(maxValue, result);
}

//Knapsack 0/1: no output iterator size
BOOST_AUTO_TEST_CASE(Knapsack_0_1_no_output_size) {
    std::vector<int> result;
    LOG("Knapsack 0/1 no output size");
    auto maxValue = paal::knapsack_0_1_no_output_size(std::begin(objects), std::end(objects), 
            capacity,
            sizesFunctor, 
            valuesFunctor);

    check_0_1_no_output(maxValue, result);
}

//Knapsack 0/1: no output iterator value
BOOST_AUTO_TEST_CASE(Knapsack_0_1_no_output_value) {
    std::vector<int> result;
    LOG("Knapsack 0/1 no output value");
    auto maxValue = paal::knapsack_0_1_no_output_value(std::begin(objects), std::end(objects), 
            capacity,
            sizesFunctor, 
            valuesFunctor);

    check_0_1_no_output(maxValue, result);
}

