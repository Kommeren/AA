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

    //TODO when c++14 template lambda appears this code can be unified with 0_1 version
    //     now the work is bigger than gain
    typedef detail::KnapsackBase<ObjectsIter, ObjectSizeFunctor, ObjectValueFunctor> base;
    typedef typename base::ObjectRef ObjectRef;
    typedef typename base::ValueType ValueType;
    typedef typename base::ReturnType ReturnType;
    if(oBegin == oEnd) {
        return ReturnType();
    }
    
    double maxValue = detail::getValueLowerBound(oBegin, oEnd, capacity, value, size, detail::NoZeroOneTag());
    auto multiplier = getMultiplier(oBegin, oEnd, epsilon, maxValue);

    if(!multiplier) {
        return knapsack(oBegin, oEnd, capacity, out, size, value);
    }

    ValueType realValue = ValueType();
    auto addValue = [&](ObjectRef obj){realValue += value(obj); return *out = obj;};

    auto newOut =  boost::make_function_output_iterator(addValue);
    
    auto newValue = [=](ObjectRef obj){return ValueType(double(value(obj)) * *multiplier); };
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
        detail::FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter> capacity, //capacity is of size type
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
    
    auto multiplier = getMultiplier(oBegin, oEnd, epsilon, capacity);

    if(!multiplier) {
        return knapsack(oBegin, oEnd, capacity, out, size, value);
    }
    SizeType realSize = SizeType();
    auto addSize = [&](ObjectRef obj){realSize += size(obj); return *out = obj;};
    
    auto newOut =  boost::make_function_output_iterator(addSize);
    
    auto newSize = [=](ObjectRef obj){return SizeType(double(size(obj)) * *multiplier); };
    auto reducedReturn = knapsack(oBegin, oEnd, SizeType(capacity / *multiplier) , newOut, newSize, value);
    return std::make_pair(reducedReturn.first, realSize);
}

} //paal


#endif /* KNAPSACK_FPTAS_HPP */
