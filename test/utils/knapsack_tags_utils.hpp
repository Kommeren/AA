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

std::string to_string(pd::IntegralValueAndSizeTag) {
    return "value and size";
}

std::string to_string(pd::IntegralValueTag) {
    return "value";
}

std::string to_string(pd::IntegralSizeTag) {
    return "size";
}

std::string to_string(pd::NoRetrieveSolutionTag) {
    return "without output";
}

std::string to_string(pd::ZeroOneTag) {
    return "0/1";
}

template <typename MaxValue> 
void printMaxValue(MaxValue maxValue) {
    LOGLN("Max value " << maxValue.first << ", Total size "  << maxValue.second);
    LOGLN("");
}

template <typename MaxValue> 
void printResult(MaxValue maxValue, const std::vector<int> & result, pd::NoRetrieveSolutionTag) {
    printMaxValue(maxValue);
}

template <typename MaxValue> 
void printResult(MaxValue maxValue, const std::vector<int> & result, pd::RetrieveSolutionTag) {
    LOG_COPY_DEL(result.begin(), result.end(), " ");
    LOGLN("");
    printMaxValue(maxValue);
}


template <typename IntegralTag, 
          typename IsZeroOne, 
          typename RetrieveSolution = pd::RetrieveSolutionTag,
          typename Objects, 
          typename ObjectSizeFunctor, 
          typename ObjectValueFunctor>
typename paal::detail::KnapsackBase<typename paal::utils::CollectionToIter<Objects>::type,
    ObjectSizeFunctor, ObjectValueFunctor>::ReturnType 
    detail_knapsack(const Objects & objects, 
                     paal::detail::FunctorOnIteratorPValue<ObjectSizeFunctor, 
                        typename paal::utils::CollectionToIter<Objects>::type> capacity, //capacity is of size type
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
    printResult(ret, result, RetrieveSolution());
    return ret;
}

struct OnValueTag {};
struct OnSizeTag {};

template <typename IsZeroOne, 
          typename RetrieveSolution = pd::RetrieveSolutionTag,
          typename Objects, 
          typename ObjectSizeFunctor, 
          typename ObjectValueFunctor>
typename paal::detail::KnapsackBase<typename paal::utils::CollectionToIter<Objects>::type,
    ObjectSizeFunctor, ObjectValueFunctor>::ReturnType 
    detail_knapsack_fptas(double epsilon, const Objects & objects, 
                     paal::detail::FunctorOnIteratorPValue<ObjectSizeFunctor, 
                        typename paal::utils::CollectionToIter<Objects>::type> capacity, //capacity is of size type
                     ObjectSizeFunctor size, 
                     ObjectValueFunctor value,
                     OnValueTag) 
{
    std::vector<int> result;
    LOGLN("Knapsack fptas epsilon " << epsilon << " " << to_string(IsZeroOne())  <<  " on value " + to_string(RetrieveSolution()));
    auto ret =  pd::knapsack_general_on_value_fptas_retrieve(epsilon, std::begin(objects), std::end(objects), 
            capacity,
            std::back_inserter(result), 
            size, 
            value,
            IsZeroOne());
    printResult(ret, result, RetrieveSolution());
    return ret;
}

template <typename IsZeroOne, 
          typename RetrieveSolution = pd::RetrieveSolutionTag,
          typename Objects, 
          typename ObjectSizeFunctor, 
          typename ObjectValueFunctor>
typename paal::detail::KnapsackBase<typename paal::utils::CollectionToIter<Objects>::type,
    ObjectSizeFunctor, ObjectValueFunctor>::ReturnType 
    detail_knapsack_fptas(double epsilon, const Objects & objects, 
                     paal::detail::FunctorOnIteratorPValue<ObjectSizeFunctor, 
                        typename paal::utils::CollectionToIter<Objects>::type> capacity, //capacity is of size type
                     ObjectSizeFunctor size, 
                     ObjectValueFunctor value,
                     OnSizeTag) 
{
    std::vector<int> result;
    LOGLN("Knapsack fptas epsilon " << epsilon << " " << to_string(IsZeroOne())  <<  " on size " + to_string(RetrieveSolution()));
    auto ret =  pd::knapsack_general_on_size_fptas_retrieve(epsilon, std::begin(objects), std::end(objects), 
            capacity,
            std::back_inserter(result), 
            size, 
            value,
            IsZeroOne());
    printResult(ret, result, RetrieveSolution());
    return ret;
}

#endif /* KNAPSACK_TAGS_UTILS_HPP */
