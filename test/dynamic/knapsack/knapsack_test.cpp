//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file knapsack_test.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-09-20
 */

#include "test_utils/logger.hpp"
#include "test_utils/knapsack_tags_utils.hpp"

#include "paal/dynamic/knapsack_unbounded.hpp"
#include "paal/dynamic/knapsack_0_1.hpp"
#include "paal/dynamic/knapsack_unbounded_fptas.hpp"
#include "paal/dynamic/knapsack_0_1_fptas.hpp"
#include "paal/utils/floating.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/fusion/include/for_each.hpp>

#include <fstream>

using namespace paal;

namespace {
const int capacity = 6;
using Objects = std::vector<std::pair<int, int>>;
Objects objects{ { 1, 3 }, { 2, 2 }, { 3, 65 }, { 1, 1 }, { 2, 2 }, { 4, 3 },
                 { 1, 1 }, { 10, 23 } };
auto size = [](std::pair<int, int> object) { return object.first; };
auto value = [](std::pair<int, int> object) { return object.second; };
}

static const double OPT = 130;
static const double OPT_0_1 = 70;
static const double OPT_CAP = 6;
static const double EPSILON = 1. / 4.;
static const double VALUE_MULTIPLIER = 1. - EPSILON;
static const double SIZE_MULTIPLIER = 1. + EPSILON;
static const utils::compare<double> compare(0.001);

template <typename MaxValue>
void check(MaxValue maxValue, pd::unbounded_tag, double valMultiplier = 1,
           double capMultiplier = 1) {
    BOOST_CHECK(compare.ge(maxValue.first, OPT * valMultiplier));
    BOOST_CHECK(compare.ge(OPT_CAP * capMultiplier, maxValue.second));
}

template <typename MaxValue>
void check(MaxValue maxValue, pd::zero_one_tag, double valMultiplier = 1,
           double capMultiplier = 1) {
    BOOST_CHECK(compare.ge(maxValue.first, OPT_0_1 * valMultiplier));
    BOOST_CHECK(compare.ge(OPT_CAP * capMultiplier, maxValue.second));
}

template <typename IntegralTag, typename IsZeroOne,
          typename RetrieveSolution = pd::retrieve_solution_tag>
void detail_knapsack_and_check() {
    auto maxValue = detail_knapsack<IntegralTag, IsZeroOne, RetrieveSolution>(
        objects, capacity, size, value);
    check(maxValue, IsZeroOne());
}

BOOST_AUTO_TEST_CASE(KnapsackOverloads) {
    detail_knapsack_and_check<pd::integral_value_tag, pd::unbounded_tag>();
    detail_knapsack_and_check<pd::integral_size_tag, pd::unbounded_tag>();
    detail_knapsack_and_check<pd::integral_value_and_size_tag,
                              pd::unbounded_tag>();
    detail_knapsack_and_check<pd::integral_value_tag, pd::zero_one_tag,
                              pd::retrieve_solution_tag>();
    detail_knapsack_and_check<pd::integral_size_tag, pd::zero_one_tag,
                              pd::retrieve_solution_tag>();
    detail_knapsack_and_check<pd::integral_value_and_size_tag, pd::zero_one_tag,
                              pd::retrieve_solution_tag>();
    detail_knapsack_and_check<pd::integral_value_tag, pd::zero_one_tag,
                              pd::no_retrieve_solution_tag>();
    detail_knapsack_and_check<pd::integral_size_tag, pd::zero_one_tag,
                              pd::no_retrieve_solution_tag>();
    detail_knapsack_and_check<pd::integral_value_and_size_tag, pd::zero_one_tag,
                              pd::no_retrieve_solution_tag>();
}

// Knapsack
BOOST_AUTO_TEST_CASE(Knapsack) {
    Objects result;
    LOGLN("Knapsack");
    auto maxValue = paal::knapsack_unbounded(
        objects, capacity, std::back_inserter(result), size, value);

    check(maxValue, pd::unbounded_tag{});
    print_result(maxValue, result, pd::retrieve_solution_tag());
}

// Knapsack 0/1
BOOST_AUTO_TEST_CASE(Knapsack_0_1) {
    Objects result;
    LOGLN("Knapsack 0/1");
    auto maxValue = paal::knapsack_0_1(objects, capacity,
                                       std::back_inserter(result), size, value);
    check(maxValue, pd::zero_one_tag());
    print_result(maxValue, result, pd::retrieve_solution_tag());
}

BOOST_AUTO_TEST_CASE(Knapsack_0_1_no_output) {
    Objects result;
    LOGLN("Knapsack 0/1 no output");
    auto maxValue =
        paal::knapsack_0_1_no_output(objects, capacity, size, value);

    check(maxValue, pd::zero_one_tag());
    print_max_value(maxValue);
}

// Knapsack fptas value
BOOST_AUTO_TEST_CASE(Knapsack_fptas_value) {
    Objects result;
    LOGLN("Knapsack fptas value");
    auto maxValue = paal::knapsack_unbounded_on_value_fptas(
        EPSILON, objects, capacity, std::back_inserter(result), size, value);

    check(maxValue, pd::unbounded_tag{}, VALUE_MULTIPLIER);
    print_result(maxValue, result, pd::retrieve_solution_tag());
}

// TODO this tests is very weak because it runs standard algorithm no fptas
// Knapsack fptas size
BOOST_AUTO_TEST_CASE(Knapsack_fptas_size) {
    Objects result;
    LOGLN("Knapsack fptas size");
    auto maxValue = paal::knapsack_unbounded_on_size_fptas(
        EPSILON, objects, capacity, std::back_inserter(result), size, value);

    print_result(maxValue, result, pd::retrieve_solution_tag());
    check(maxValue, pd::unbounded_tag{}, 1., SIZE_MULTIPLIER);
}

// Knapsack 0/1: no output iterator size fptas
BOOST_AUTO_TEST_CASE(Knapsack_0_1_no_output_size_fptas) {
    Objects result;
    LOGLN("Knapsack 0/1 no output size fptas");
    auto maxValue = paal::knapsack_0_1_no_output_on_size_fptas(
        EPSILON, objects, capacity, size, value);

    check(maxValue, pd::zero_one_tag(), 1, SIZE_MULTIPLIER);
    print_max_value(maxValue);
}

// Knapsack 0/1: no output iterator value fptas
BOOST_AUTO_TEST_CASE(Knapsack_0_1_no_output_value_fptas) {
    Objects result;
    LOGLN("Knapsack 0/1 no output value fptas");
    auto maxValue = paal::knapsack_0_1_no_output_on_value_fptas(
        EPSILON, objects, capacity, size, value);

    check(maxValue, pd::zero_one_tag(), VALUE_MULTIPLIER);
    print_max_value(maxValue);
}

// Knapsack 0/1 value fptas
BOOST_AUTO_TEST_CASE(Knapsack_0_1_value_fptas) {
    Objects result;
    LOGLN("Knapsack 0/1 on value fptas");
    auto maxValue = paal::knapsack_0_1_on_value_fptas(
        EPSILON, objects, capacity, std::back_inserter(result), size, value);

    check(maxValue, pd::zero_one_tag(), VALUE_MULTIPLIER);
    print_result(maxValue, result, pd::retrieve_solution_tag());
}

// Knapsack 0/1  size fptas
BOOST_AUTO_TEST_CASE(Knapsack_0_1_size_fptas) {
    Objects result;
    LOGLN("Knapsack 0/1 on size fptas");
    auto maxValue = paal::knapsack_0_1_on_size_fptas(
        EPSILON, objects, capacity, std::back_inserter(result), size, value);

    check(maxValue, pd::zero_one_tag(), 1, SIZE_MULTIPLIER);
    print_result(maxValue, result, pd::retrieve_solution_tag());
}
