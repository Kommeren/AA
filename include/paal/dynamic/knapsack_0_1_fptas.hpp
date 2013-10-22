/**
 * @file knapsack_0_1_fptas.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-10-04
 */
#ifndef KNAPSACK_0_1_FPTAS_HPP
#define KNAPSACK_0_1_FPTAS_HPP 
#include "paal/dynamic/knapsack.hpp"

namespace paal {

template <typename OutputIterator, 
          typename ObjectsIter, 
          typename ObjectSizeFunctor, 
          typename ObjectValueFunctor>
typename detail::KnapsackBase<ObjectsIter, ObjectSizeFunctor, ObjectValueFunctor>::ReturnType
knapsack_0_1_on_value_fptas(double epsilon, ObjectsIter oBegin, 
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
        return knapsack_0_1(oBegin, oEnd, capacity, out, size, value);
    }
    
    ValueType realValue = ValueType();
    auto addValue = [&](ObjectRef obj){realValue += value(obj); return *out = obj;};

    auto newOut =  utils::make_FunctorToOutputIterator(addValue);
    
    auto newValue = [=](ObjectRef obj){return ValueType(double(value(obj)) * multiplier); };

    auto reducedReturn = knapsack_0_1(oBegin, oEnd, capacity, newOut, size, newValue);
    return std::make_pair(realValue, reducedReturn.second);
}

template <typename OutputIterator, 
          typename ObjectsIter, 
          typename ObjectSizeFunctor, 
          typename ObjectValueFunctor>
typename detail::KnapsackBase<ObjectsIter, ObjectSizeFunctor, ObjectValueFunctor>::ReturnType
knapsack_0_1_on_size_fptas(double epsilon, ObjectsIter oBegin, 
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
        return knapsack_0_1(oBegin, oEnd, capacity, out, size, value);
    }
    
    auto newSize = [=](ObjectRef obj){return SizeType(double(size(obj)) * multiplier); };
    return knapsack_0_1(oBegin, oEnd, SizeType(capacity / multiplier) , out, newSize, value);
}

template <typename ObjectsIter, 
          typename ObjectSizeFunctor, 
          typename ObjectValueFunctor>
typename detail::KnapsackBase<ObjectsIter, ObjectSizeFunctor, ObjectValueFunctor>::ReturnType
knapsack_0_1_no_output_on_value_fptas(double epsilon, ObjectsIter oBegin, 
        ObjectsIter oEnd, 
        puretype(std::declval<ObjectSizeFunctor>()(*std::declval<ObjectsIter>())) capacity,
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
        return knapsack_0_1_no_output(oBegin, oEnd, capacity, size, value);
    }
    
    auto newValue = [=](ObjectRef obj){return ValueType(double(value(obj)) * multiplier); };
    auto ret = knapsack_0_1_no_output(oBegin, oEnd, capacity, size, newValue);
    return std::make_pair((double(ret.first) / multiplier), ret.second);
}

template <typename ObjectsIter, 
          typename ObjectSizeFunctor, 
          typename ObjectValueFunctor>
typename detail::KnapsackBase<ObjectsIter, ObjectSizeFunctor, ObjectValueFunctor>::ReturnType
knapsack_0_1_no_output_on_size_fptas(double epsilon, ObjectsIter oBegin, 
        ObjectsIter oEnd, 
        puretype(std::declval<ObjectSizeFunctor>()(*std::declval<ObjectsIter>())) capacity,
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
        return knapsack_0_1_no_output(oBegin, oEnd, capacity, size, value);
    }
    
    auto newSize = [=](ObjectRef obj){return SizeType(double(size(obj)) * multiplier); };
    return knapsack_0_1_no_output(oBegin, oEnd, SizeType(capacity / multiplier), newSize, value);
}

} //paal


#endif /* KNAPSACK_0_1_FPTAS_HPP */
