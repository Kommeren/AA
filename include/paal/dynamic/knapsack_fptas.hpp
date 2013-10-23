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

namespace paal {

template <typename OutputIterator, 
          typename ObjectsIter, 
          typename ObjectSizeFunctor, 
          typename ObjectValueFunctor>
typename detail::KnapsackBase<ObjectsIter, ObjectSizeFunctor, ObjectValueFunctor>::ReturnType
knapsack_on_value_fptas(double epsilon, ObjectsIter oBegin, 
        ObjectsIter oEnd, 
        puretype(std::declval<ObjectSizeFunctor>()(*std::declval<ObjectsIter>())) capacity,
        OutputIterator out, 
        ObjectSizeFunctor size, 
        ObjectValueFunctor value) {
    typedef detail::KnapsackBase<ObjectsIter, ObjectSizeFunctor, ObjectValueFunctor> base;
    typedef typename base::ObjectRef ObjectRef;
    typedef typename base::ValueType ValueType;
    typedef typename base::ReturnType ReturnType;
    if(oBegin == oEnd) {
        return ReturnType();
    }
    
    double n = std::distance(oBegin, oEnd);
    //TODO use better guarantee
    double maxValue = value(*std::max_element(oBegin, oEnd, utils::make_FunctorToComparator(value)));
    auto multiplier = n / (epsilon * maxValue);
    static const double SMALLEST_MULTIPLIER = 1./2.;

    if(multiplier  > SMALLEST_MULTIPLIER) {
        return knapsack(oBegin, oEnd, capacity, out, size, value);
    }


    ValueType realValue = ValueType();
    auto addValue = [&](ObjectRef obj){realValue += value(obj); return *out = obj;};

    auto newOut =  boost::make_function_output_iterator(addValue);
    
    auto newValue = [=](ObjectRef obj){return ValueType(double(value(obj)) * multiplier); };
    auto reducedReturn = knapsack(oBegin, oEnd, capacity, newOut, size, newValue);
    return std::make_pair(realValue, reducedReturn.second);
}

template <typename OutputIterator, 
          typename ObjectsIter, 
          typename ObjectSizeFunctor, 
          typename ObjectValueFunctor>
typename detail::KnapsackBase<ObjectsIter, ObjectSizeFunctor, ObjectValueFunctor>::ReturnType
knapsack_on_size_fptas(double epsilon, ObjectsIter oBegin, 
        ObjectsIter oEnd, 
        puretype(std::declval<ObjectSizeFunctor>()(*std::declval<ObjectsIter>())) capacity,
        OutputIterator out, 
        ObjectSizeFunctor size, 
        ObjectValueFunctor value) {
    typedef detail::KnapsackBase<ObjectsIter, ObjectSizeFunctor, ObjectValueFunctor> base;
    typedef typename base::ObjectRef ObjectRef;
    typedef typename base::SizeType SizeType;
    typedef typename base::ReturnType ReturnType;
    if(oBegin == oEnd) {
        return ReturnType();
    }
    
    double n = std::distance(oBegin, oEnd);
    auto multiplier = n / (epsilon * double(capacity));
    static const double SMALLEST_MULTIPLIER = 1./2.;

    if(multiplier > SMALLEST_MULTIPLIER) {
        return knapsack(oBegin, oEnd, capacity, out, size, value);
    }
    
    auto newSize = [=](ObjectRef obj){return SizeType(double(size(obj)) * multiplier); };
    return knapsack(oBegin, oEnd, SizeType(capacity / multiplier) , out, newSize, value);
}

} //paal


#endif /* KNAPSACK_FPTAS_HPP */
