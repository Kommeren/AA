/**
 * @file knapsack_fptas.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-10-01
 */
#ifndef KNAPSACK_FPTAS_HPP
#define KNAPSACK_FPTAS_HPP 

#include <boost/function_output_iterator.hpp>

#include "paal/dynamic/knapsack.hpp"
#include "paal/dynamic/knapsack/get_lower_bound.hpp"
#include "paal/dynamic/knapsack/knapsack_fptas_common.hpp"

#include "paal/greedy/knapsack_two_app.hpp"

namespace paal {

template <typename OutputIterator, 
          typename ObjectsIter, 
          typename ObjectSizeFunctor, 
          typename ObjectValueFunctor>
typename detail::KnapsackBase<ObjectsIter, ObjectSizeFunctor, ObjectValueFunctor>::ReturnType
knapsack_on_value_fptas(double epsilon, ObjectsIter oBegin, 
        ObjectsIter oEnd, 
        detail::FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter> capacity, //capacity is of size type
        OutputIterator out, 
        ObjectSizeFunctor size, 
        ObjectValueFunctor value) {
    return detail::knapsack_general_on_value_fptas_retrieve(
              epsilon, oBegin, oEnd, capacity, out, size, value, detail::NoZeroOneTag());
}

template <typename OutputIterator, 
          typename ObjectsIter, 
          typename ObjectSizeFunctor, 
          typename ObjectValueFunctor>
typename detail::KnapsackBase<ObjectsIter, ObjectSizeFunctor, ObjectValueFunctor>::ReturnType
knapsack_on_size_fptas(double epsilon, ObjectsIter oBegin, 
        ObjectsIter oEnd, 
        detail::FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter> capacity, //capacity is of size type
        OutputIterator out, 
        ObjectSizeFunctor size, 
        ObjectValueFunctor value) {
    return detail::knapsack_general_on_size_fptas_retrieve(
              epsilon, oBegin, oEnd, capacity, out, size, value, detail::NoZeroOneTag());
}

} //paal


#endif /* KNAPSACK_FPTAS_HPP */
