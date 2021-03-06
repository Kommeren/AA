//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file knapsack_tags_utils.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-11-05
 */
#ifndef PAAL_KNAPSACK_TAGS_UTILS_HPP
#define PAAL_KNAPSACK_TAGS_UTILS_HPP

#include "paal/dynamic/knapsack/knapsack_fptas_common.hpp"
#include "paal/dynamic/knapsack_unbounded_fptas.hpp"
#include "paal/dynamic/knapsack_0_1_fptas.hpp"

namespace pd = paal::detail;

template <typename T> std::string to_string(T) { return ""; }

inline std::string to_string(pd::integral_value_and_size_tag) {
    return "value and size";
}

inline std::string to_string(pd::integral_value_tag) { return "value"; }

inline std::string to_string(pd::integral_size_tag) { return "size"; }

inline std::string to_string(pd::no_retrieve_solution_tag) { return "without output"; }

inline std::string to_string(pd::zero_one_tag) { return "0/1"; }

template <typename MaxValue> void print_max_value(MaxValue maxValue) {
    LOGLN("Max value " << maxValue.first << ", Total size " << maxValue.second);
    LOGLN("");
}

template <typename MaxValue>
void print_result(MaxValue maxValue,
                  const std::vector<std::pair<int, int>> &result,
                  pd::no_retrieve_solution_tag) {
    print_max_value(maxValue);
}

template <typename MaxValue>
void print_result(MaxValue maxValue,
                  const std::vector<std::pair<int, int>> &result,
                  pd::retrieve_solution_tag) {
    ON_LOG(for (auto o
                : result) {
        std::cout << "{ size = " << o.first << ", value = " << o.second << "} ";
    });
    LOGLN("");
    print_max_value(maxValue);
}

template <typename IntegralTag, typename IsZeroOne,
          typename RetrieveSolution = pd::retrieve_solution_tag,
          typename Objects, typename ObjectSizeFunctor,
          typename ObjectValueFunctor,
          typename Base = typename paal::detail::knapsack_base<
              Objects, ObjectSizeFunctor, ObjectValueFunctor>>
typename Base::return_type detail_knapsack(Objects &objects,
                                           typename Base::SizeType capacity,
                                           ObjectSizeFunctor size,
                                           ObjectValueFunctor value) {
    std::vector<std::pair<int, int>> result;
    LOGLN("Knapsack " << to_string(IsZeroOne())
                      << " on " + to_string(IntegralTag()) + " " +
                             to_string(RetrieveSolution()));
    auto out = std::back_inserter(result);
    auto ret =
        pd::knapsack(pd::make_knapsack_data(objects, capacity, size, value, out),
                     IsZeroOne(), IntegralTag(), RetrieveSolution());
    print_result(ret, result, RetrieveSolution());
    return ret;
}

struct on_value_tag {};
struct on_size_tag {};

template <
    typename IsZeroOne, typename RetrieveSolution = pd::retrieve_solution_tag,
    typename Objects, typename ObjectSizeFunctor, typename ObjectValueFunctor,
    typename Base = typename paal::detail::knapsack_base<
        Objects, ObjectSizeFunctor, ObjectValueFunctor>>
typename Base::return_type
detail_knapsack_fptas(double epsilon, Objects &objects,
                      typename Base::SizeType capacity, ObjectSizeFunctor size,
                      ObjectValueFunctor value, on_value_tag) {
    std::vector<std::pair<int, int>> result;
    auto out = std::back_inserter(result);
    LOGLN("Knapsack fptas epsilon "
          << epsilon << " " << to_string(IsZeroOne())
          << " on value " + to_string(RetrieveSolution()));
    auto ret = pd::knapsack_general_on_value_fptas_retrieve(
        epsilon, pd::make_knapsack_data(objects, capacity, size, value, out),
        IsZeroOne{});
    print_result(ret, result, RetrieveSolution());
    return ret;
}

template <
    typename IsZeroOne, typename RetrieveSolution = pd::retrieve_solution_tag,
    typename Objects, typename ObjectSizeFunctor, typename ObjectValueFunctor,
    typename Base = typename paal::detail::knapsack_base<
        Objects, ObjectSizeFunctor, ObjectValueFunctor>>
typename Base::return_type
detail_knapsack_fptas(double epsilon, Objects &objects,
                      typename Base::SizeType capacity, ObjectSizeFunctor size,
                      ObjectValueFunctor value, on_size_tag) {
    std::vector<std::pair<int, int>> result;
    LOGLN("Knapsack fptas epsilon "
          << epsilon << " " << to_string(IsZeroOne())
          << " on size " + to_string(RetrieveSolution()));
    auto out = std::back_inserter(result);
    auto ret = pd::knapsack_general_on_size_fptas_retrieve(
        epsilon, pd::make_knapsack_data(objects, capacity, size, value,out),
        IsZeroOne{});
    print_result(ret, result, RetrieveSolution());
    return ret;
}

#endif // PAAL_KNAPSACK_TAGS_UTILS_HPP
