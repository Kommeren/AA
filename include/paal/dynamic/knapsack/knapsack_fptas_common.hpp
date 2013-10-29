/**
 * @file knapsack_fptas_common.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-10-04
 */
#ifndef KNAPSACK_FPTAS_COMMON_HPP
#define KNAPSACK_FPTAS_COMMON_HPP 
namespace paal {
namespace detail {

template <typename OutputIterator, 
          typename ObjectsIter, 
          typename ObjectSizeFunctor, 
          typename ObjectValueFunctor,
          typename IsZeroOne,
          typename RetrieveSolution> 
typename detail::KnapsackBase<ObjectsIter, ObjectSizeFunctor, ObjectValueFunctor>::ReturnType
knapsack_general_on_value_fptas(double epsilon, ObjectsIter oBegin, 
        ObjectsIter oEnd, 
        detail::FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter> capacity, //capacity is of size type
        OutputIterator out, 
        ObjectSizeFunctor size, 
        ObjectValueFunctor value,
        IsZeroOne is_0_1_Tag,
        RetrieveSolution retrieveSolution) {
    typedef detail::KnapsackBase<ObjectsIter, ObjectSizeFunctor, ObjectValueFunctor> base;
    typedef typename base::ObjectRef ObjectRef;
    typedef typename base::ValueType ValueType;
    typedef typename base::ReturnType ReturnType;
    if(oBegin == oEnd) {
        return ReturnType();
    }
    
    double maxValue = detail::getValueLowerBound(oBegin, oEnd, capacity, value, size, is_0_1_Tag); 
    auto multiplier = getMultiplier(oBegin, oEnd, epsilon, maxValue);

    if(!multiplier) {
        return knapsack_check_integrality(oBegin, oEnd, capacity, out, size, value, is_0_1_Tag, retrieveSolution);
    }
    
    auto newValue = [=](ObjectRef obj){return ValueType(double(value(obj)) * *multiplier); };
    auto ret = knapsack_check_integrality(oBegin, oEnd, capacity, out, size, newValue, is_0_1_Tag, retrieveSolution);
    return std::make_pair(ValueType(double(ret.first) / *multiplier), ret.second);
}

template <typename OutputIterator, 
          typename ObjectsIter, 
          typename ObjectSizeFunctor, 
          typename ObjectValueFunctor,
          typename IsZeroOne,
          typename RetrieveSolution> 
typename KnapsackBase<ObjectsIter, ObjectSizeFunctor, ObjectValueFunctor>::ReturnType
knapsack_general_on_size_fptas(double epsilon, ObjectsIter oBegin, 
        ObjectsIter oEnd, 
        FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter> capacity, //capacity is of size type
        OutputIterator out, 
        ObjectSizeFunctor size, 
        ObjectValueFunctor value,
        IsZeroOne is_0_1_Tag,
        RetrieveSolution retrieveSolution) {
    typedef KnapsackBase<ObjectsIter, ObjectSizeFunctor, ObjectValueFunctor> base;
    typedef typename base::ObjectRef ObjectRef;
    typedef typename base::SizeType SizeType;
    typedef typename base::ReturnType ReturnType;
    if(oBegin == oEnd) {
        return ReturnType();
    }
    
    auto multiplier = getMultiplier(oBegin, oEnd, epsilon, capacity);

    if(!multiplier) {
        return knapsack_check_integrality(oBegin, oEnd, capacity, out, size, value, is_0_1_Tag, retrieveSolution);
    }
    
    auto newSize = [=](ObjectRef obj){return SizeType(double(size(obj)) * *multiplier); };
    auto ret = knapsack_check_integrality(oBegin, oEnd, SizeType(capacity / *multiplier), out, 
                newSize, value, is_0_1_Tag, retrieveSolution);
    return ReturnType(ret.first, double(ret.second) / *multiplier);
}


template <typename OutputIterator, 
          typename ObjectsIter, 
          typename ObjectSizeFunctor, 
          typename ObjectValueFunctor,
          typename IsZeroOne>
typename KnapsackBase<ObjectsIter, ObjectSizeFunctor, ObjectValueFunctor>::ReturnType
knapsack_general_on_value_fptas_retrieve(double epsilon, ObjectsIter oBegin, 
        ObjectsIter oEnd, 
        FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter> capacity, //capacity is of size type
        OutputIterator out, 
        ObjectSizeFunctor size, 
        ObjectValueFunctor value,
        IsZeroOne is_0_1_Tag) {
    typedef detail::KnapsackBase<ObjectsIter, ObjectSizeFunctor, ObjectValueFunctor> base;
    typedef typename base::ObjectRef ObjectRef;
    typedef typename base::ValueType ValueType;
    
    ValueType realValue = ValueType();
    auto addValue = [&](ObjectRef obj){realValue += value(obj); return *out = obj;};

    auto newOut =  boost::make_function_output_iterator(addValue);

    auto reducedReturn = knapsack_general_on_value_fptas(
            epsilon, oBegin, oEnd, capacity, newOut, size, value, is_0_1_Tag, RetrieveSolutionTag());
    return std::make_pair(realValue, reducedReturn.second);
}

template <typename OutputIterator, 
          typename ObjectsIter, 
          typename ObjectSizeFunctor, 
          typename ObjectValueFunctor,
          typename IsZeroOne>
typename detail::KnapsackBase<ObjectsIter, ObjectSizeFunctor, ObjectValueFunctor>::ReturnType
knapsack_general_on_size_fptas_retrieve(double epsilon, ObjectsIter oBegin, 
        ObjectsIter oEnd, 
        detail::FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter> capacity, //capacity is of size type
        OutputIterator out, 
        ObjectSizeFunctor size, 
        ObjectValueFunctor value,
        IsZeroOne is_0_1_Tag) {
    typedef detail::KnapsackBase<ObjectsIter, ObjectSizeFunctor, ObjectValueFunctor> base;
    typedef typename base::ObjectRef ObjectRef;
    typedef typename base::SizeType SizeType;
    
    SizeType realSize = SizeType();
    auto addSize = [&](ObjectRef obj){realSize += size(obj); return *out = obj;};
    
    auto newOut =  boost::make_function_output_iterator(addSize);
    
    auto reducedReturn = knapsack_general_on_size_fptas(
            epsilon, oBegin, oEnd, capacity, newOut, size, value, is_0_1_Tag, RetrieveSolutionTag());
    return std::make_pair(reducedReturn.first, realSize);
}



}//detail
}//paal

#endif /* KNAPSACK_FPTAS_COMMON_HPP */
