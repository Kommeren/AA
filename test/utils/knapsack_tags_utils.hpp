/**
 * @file knapsack_tags_utils.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-11-05
 */
#ifndef KNAPSACK_TAGS_UTILS_HPP
#define KNAPSACK_TAGS_UTILS_HPP

#include "paal/dynamic/knapsack/knapsack_fptas_common.hpp"
#include "paal/dynamic/knapsack_fptas.hpp"
#include "paal/dynamic/knapsack_0_1_fptas.hpp"

namespace pd = paal::detail;

template <typename T>
std::string to_string(T) {
    return "";
}

std::string to_string(pd::integral_value_and_size_tag) {
    return "value and size";
}

std::string to_string(pd::integral_value_tag) {
    return "value";
}

std::string to_string(pd::integral_size_tag) {
    return "size";
}

std::string to_string(pd::no_retrieve_solution_tag) {
    return "without output";
}

std::string to_string(pd::zero_one_tag) {
    return "0/1";
}

template <typename MaxValue>
void print_max_value(MaxValue maxValue) {
    LOGLN("Max value " << maxValue.first << ", Total size "  << maxValue.second);
    LOGLN("");
}

template <typename MaxValue>
void print_result(MaxValue maxValue, const std::vector<int> & result, pd::no_retrieve_solution_tag) {
    print_max_value(maxValue);
}

template <typename MaxValue>
void print_result(MaxValue maxValue, const std::vector<int> & result, pd::retrieve_solution_tag) {
    LOG_COPY_RANGE_DEL(result, " ");
    LOGLN("");
    print_max_value(maxValue);
}


template <typename IntegralTag,
          typename IsZeroOne,
          typename RetrieveSolution = pd::retrieve_solution_tag,
          typename Objects,
          typename ObjectSizeFunctor,
          typename ObjectValueFunctor>
typename paal::detail::knapsack_base<typename boost::range_iterator<Objects>::type,
    ObjectSizeFunctor, ObjectValueFunctor>::return_type
    detail_knapsack(const Objects & objects,
                     paal::detail::FunctorOnIteratorPValue<ObjectSizeFunctor,
                        typename boost::range_iterator<Objects>::type> capacity, //capacity is of size type
                     ObjectSizeFunctor size,
                     ObjectValueFunctor value)
{
    std::vector<int> result;
    LOGLN("Knapsack " << to_string(IsZeroOne())  <<  " on " + to_string(IntegralTag()) + " " + to_string(RetrieveSolution()));
    auto ret =  pd::knapsack(std::begin(objects), std::end(objects),
            capacity,
            std::back_inserter(result),
            size,
            value,
            IsZeroOne(),
            IntegralTag(),
            RetrieveSolution());
    print_result(ret, result, RetrieveSolution());
    return ret;
}

struct on_value_tag {};
struct on_size_tag {};

template <typename IsZeroOne,
          typename RetrieveSolution = pd::retrieve_solution_tag,
          typename Objects,
          typename ObjectSizeFunctor,
          typename ObjectValueFunctor>
typename paal::detail::knapsack_base<typename boost::range_iterator<Objects>::type,
    ObjectSizeFunctor, ObjectValueFunctor>::return_type
    detail_knapsack_fptas(double epsilon, const Objects & objects,
                     paal::detail::FunctorOnIteratorPValue<ObjectSizeFunctor,
                        typename boost::range_iterator<Objects>::type> capacity, //capacity is of size type
                     ObjectSizeFunctor size,
                     ObjectValueFunctor value,
                     on_value_tag)
{
    std::vector<int> result;
    LOGLN("Knapsack fptas epsilon " << epsilon << " " << to_string(IsZeroOne())  <<  " on value " + to_string(RetrieveSolution()));
    auto ret =  pd::knapsack_general_on_value_fptas_retrieve(epsilon, std::begin(objects), std::end(objects),
            capacity,
            std::back_inserter(result),
            size,
            value,
            IsZeroOne());
    print_result(ret, result, RetrieveSolution());
    return ret;
}

template <typename IsZeroOne,
          typename RetrieveSolution = pd::retrieve_solution_tag,
          typename Objects,
          typename ObjectSizeFunctor,
          typename ObjectValueFunctor>
typename paal::detail::knapsack_base<typename boost::range_iterator<Objects>::type,
    ObjectSizeFunctor, ObjectValueFunctor>::return_type
    detail_knapsack_fptas(double epsilon, const Objects & objects,
                     paal::detail::FunctorOnIteratorPValue<ObjectSizeFunctor,
                        typename boost::range_iterator<Objects>::type> capacity, //capacity is of size type
                     ObjectSizeFunctor size,
                     ObjectValueFunctor value,
                     on_size_tag)
{
    std::vector<int> result;
    LOGLN("Knapsack fptas epsilon " << epsilon << " " << to_string(IsZeroOne())  <<  " on size " + to_string(RetrieveSolution()));
    auto ret =  pd::knapsack_general_on_size_fptas_retrieve(epsilon, std::begin(objects), std::end(objects),
            capacity,
            std::back_inserter(result),
            size,
            value,
            IsZeroOne());
    print_result(ret, result, RetrieveSolution());
    return ret;
}

#endif /* KNAPSACK_TAGS_UTILS_HPP */
