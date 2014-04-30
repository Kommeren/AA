/**
 * @file knapsack_fptas_common.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-10-04
 */
#ifndef KNAPSACK_FPTAS_COMMON_HPP
#define KNAPSACK_FPTAS_COMMON_HPP


#include "paal/dynamic/knapsack/get_lower_bound.hpp"



namespace paal {
namespace detail {

template <typename OutputIterator,
          typename ObjectsIter,
          typename ObjectSizeFunctor,
          typename ObjectValueFunctor,
          typename IsZeroOne,
          typename RetrieveSolution>
typename detail::knapsack_base<ObjectsIter, ObjectSizeFunctor, ObjectValueFunctor>::return_type
knapsack_general_on_value_fptas(double epsilon, ObjectsIter oBegin,
        ObjectsIter oEnd,
        detail::FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter> capacity, //capacity is of size type
        OutputIterator out,
        ObjectSizeFunctor size,
        ObjectValueFunctor value,
        IsZeroOne is_0_1_Tag,
        RetrieveSolution retrieve_solution) {
    typedef detail::knapsack_base<ObjectsIter, ObjectSizeFunctor, ObjectValueFunctor> base;
    typedef typename base::ObjectRef ObjectRef;
    typedef typename base::ValueType ValueType;
    typedef typename base::return_type return_type;
    if(oBegin == oEnd) {
        return return_type();
    }

    double maxValue = detail::get_value_lower_bound(oBegin, oEnd, capacity, value, size, is_0_1_Tag);
    auto multiplier = get_multiplier(oBegin, oEnd, epsilon, maxValue, value, is_0_1_Tag);

    if(!multiplier) {
        return knapsack_check_integrality(oBegin, oEnd, capacity, out, size, value, is_0_1_Tag, retrieve_solution);
    }

    auto newValue = utils::make_scale_functor<double, ValueType>(value, *multiplier);
    auto ret = knapsack_check_integrality(oBegin, oEnd, capacity, out, size, newValue, is_0_1_Tag, retrieve_solution);
    return std::make_pair(ValueType(double(ret.first) / *multiplier), ret.second);
}

template <typename OutputIterator,
          typename ObjectsIter,
          typename ObjectSizeFunctor,
          typename ObjectValueFunctor,
          typename IsZeroOne,
          typename RetrieveSolution>
typename knapsack_base<ObjectsIter, ObjectSizeFunctor, ObjectValueFunctor>::return_type
knapsack_general_on_size_fptas(double epsilon, ObjectsIter oBegin,
        ObjectsIter oEnd,
        FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter> capacity, //capacity is of size type
        OutputIterator out,
        ObjectSizeFunctor size,
        ObjectValueFunctor value,
        IsZeroOne is_0_1_Tag,
        RetrieveSolution retrieve_solution) {
    typedef knapsack_base<ObjectsIter, ObjectSizeFunctor, ObjectValueFunctor> base;
    typedef typename base::ObjectRef ObjectRef;
    typedef typename base::SizeType SizeType;
    typedef typename base::return_type return_type;
    if(oBegin == oEnd) {
        return return_type();
    }

    auto multiplier = get_multiplier(oBegin, oEnd, epsilon, capacity, size, is_0_1_Tag);

    if(!multiplier) {
        return knapsack_check_integrality(oBegin, oEnd, capacity, out, size, value, is_0_1_Tag, retrieve_solution);
    }

    auto newSize = utils::make_scale_functor<double, SizeType>(size, *multiplier);
    auto ret = knapsack_check_integrality(oBegin, oEnd, SizeType(capacity * *multiplier), out,
                newSize, value, is_0_1_Tag, retrieve_solution);
    return return_type(ret.first, double(ret.second) / *multiplier);
}


template <typename OutputIterator,
          typename ObjectsIter,
          typename ObjectSizeFunctor,
          typename ObjectValueFunctor,
          typename IsZeroOne>
typename knapsack_base<ObjectsIter, ObjectSizeFunctor, ObjectValueFunctor>::return_type
knapsack_general_on_value_fptas_retrieve(double epsilon, ObjectsIter oBegin,
        ObjectsIter oEnd,
        FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter> capacity, //capacity is of size type
        OutputIterator out,
        ObjectSizeFunctor size,
        ObjectValueFunctor value,
        IsZeroOne is_0_1_Tag) {
    typedef detail::knapsack_base<ObjectsIter, ObjectSizeFunctor, ObjectValueFunctor> base;
    typedef typename base::ObjectRef ObjectRef;
    typedef typename base::ValueType ValueType;

    ValueType realValue = ValueType();
    auto addValue = [&](ObjectRef obj){realValue += value(obj); return *out = obj;};

    auto newOut =  boost::make_function_output_iterator(addValue);

    auto reducedReturn = knapsack_general_on_value_fptas(
            epsilon, oBegin, oEnd, capacity, newOut, size, value, is_0_1_Tag, retrieve_solution_tag());
    return std::make_pair(realValue, reducedReturn.second);
}

template <typename OutputIterator,
          typename ObjectsIter,
          typename ObjectSizeFunctor,
          typename ObjectValueFunctor,
          typename IsZeroOne>
typename detail::knapsack_base<ObjectsIter, ObjectSizeFunctor, ObjectValueFunctor>::return_type
knapsack_general_on_size_fptas_retrieve(double epsilon, ObjectsIter oBegin,
        ObjectsIter oEnd,
        detail::FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter> capacity, //capacity is of size type
        OutputIterator out,
        ObjectSizeFunctor size,
        ObjectValueFunctor value,
        IsZeroOne is_0_1_Tag) {
    typedef detail::knapsack_base<ObjectsIter, ObjectSizeFunctor, ObjectValueFunctor> base;
    typedef typename base::ObjectRef ObjectRef;
    typedef typename base::SizeType SizeType;

    SizeType realSize = SizeType();
    auto addSize = [&](ObjectRef obj){realSize += size(obj); return *out = obj;};

    auto newOut =  boost::make_function_output_iterator(addSize);

    auto reducedReturn = knapsack_general_on_size_fptas(
            epsilon, oBegin, oEnd, capacity, newOut, size, value, is_0_1_Tag, retrieve_solution_tag());
    return std::make_pair(reducedReturn.first, realSize);
}



}//detail
}//paal

#endif /* KNAPSACK_FPTAS_COMMON_HPP */
