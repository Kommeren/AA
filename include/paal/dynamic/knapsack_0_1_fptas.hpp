/**
 * @file knapsack_0_1_fptas.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-10-04
 */
#ifndef KNAPSACK_0_1_FPTAS_HPP
#define KNAPSACK_0_1_FPTAS_HPP 

#include "paal/dynamic/knapsack_0_1.hpp"
#include "paal/dynamic/knapsack/get_lower_bound.hpp"
#include "paal/dynamic/knapsack/knapsack_fptas_common.hpp"

namespace paal {

template <typename OutputIterator, 
          typename ObjectsIter, 
          typename ObjectSizeFunctor, 
          typename ObjectValueFunctor>
typename detail::KnapsackBase<ObjectsIter, ObjectSizeFunctor, ObjectValueFunctor>::ReturnType
knapsack_0_1_on_value_fptas(double epsilon, ObjectsIter oBegin, 
        ObjectsIter oEnd, 
        detail::FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter> capacity, //capacity is of size type
        OutputIterator out, 
        ObjectSizeFunctor size, 
        ObjectValueFunctor value) {
    return detail::knapsack_general_on_value_fptas_retrieve(
              epsilon, oBegin, oEnd, capacity, out, size, value, detail::ZeroOneTag());
}

template <typename OutputIterator, 
          typename ObjectsIter, 
          typename ObjectSizeFunctor, 
          typename ObjectValueFunctor>
typename detail::KnapsackBase<ObjectsIter, ObjectSizeFunctor, ObjectValueFunctor>::ReturnType
knapsack_0_1_on_size_fptas(double epsilon, ObjectsIter oBegin, 
        ObjectsIter oEnd, 
        detail::FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter> capacity, //capacity is of size type
        OutputIterator out, 
        ObjectSizeFunctor size, 
        ObjectValueFunctor value) {
    return detail::knapsack_general_on_size_fptas_retrieve(
              epsilon, oBegin, oEnd, capacity, out, size, value, detail::ZeroOneTag());
}

template <typename ObjectsIter, 
          typename ObjectSizeFunctor, 
          typename ObjectValueFunctor>
typename detail::KnapsackBase<ObjectsIter, ObjectSizeFunctor, ObjectValueFunctor>::ReturnType
knapsack_0_1_no_output_on_value_fptas(double epsilon, ObjectsIter oBegin, 
        ObjectsIter oEnd, 
        detail::FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter> capacity, //capacity is of size type
        ObjectSizeFunctor size, 
        ObjectValueFunctor value) {
    auto out = boost::make_function_output_iterator(utils::SkipFunctor());
    return detail::knapsack_general_on_value_fptas(epsilon, oBegin, oEnd, 
            capacity, out, size, value, detail::ZeroOneTag(), detail::NoRetrieveSolutionTag());
}

template <typename ObjectsIter, 
          typename ObjectSizeFunctor, 
          typename ObjectValueFunctor>
typename detail::KnapsackBase<ObjectsIter, ObjectSizeFunctor, ObjectValueFunctor>::ReturnType
knapsack_0_1_no_output_on_size_fptas(double epsilon, ObjectsIter oBegin, 
        ObjectsIter oEnd, 
        detail::FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter> capacity, //capacity is of size type
        ObjectSizeFunctor size, 
        ObjectValueFunctor value) {
    auto out = boost::make_function_output_iterator(utils::SkipFunctor());
    return detail::knapsack_general_on_size_fptas(epsilon, oBegin, oEnd, 
            capacity, out, size, value, detail::ZeroOneTag(), detail::NoRetrieveSolutionTag());
}

} //paal


#endif /* KNAPSACK_0_1_FPTAS_HPP */
