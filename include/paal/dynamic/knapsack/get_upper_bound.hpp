/**
 * @file get_upper_bound.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-10-04
 */
#ifndef GET_UPPER_BOUND_HPP
#define GET_UPPER_BOUND_HPP

#include <boost/function_output_iterator.hpp>

#include "paal/utils/knapsack_utils.hpp"
#include "paal/greedy/knapsack/knapsack_general.hpp"


namespace paal {
namespace detail {

    template <typename ObjectsIter,
              typename ObjectSizeFunctor,
              typename ObjectValueFunctor,
              typename Is_0_1_Tag>
    FunctorOnIteratorPValue<ObjectValueFunctor, ObjectsIter>
    getValueUpperBound(ObjectsIter oBegin, ObjectsIter oEnd,
     FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter> capacity, //capacity is of size type
     ObjectValueFunctor value, ObjectSizeFunctor size, NonArithmeticSizeTag, Is_0_1_Tag) {
         return getDensityBasedValueUpperBound(oBegin, oEnd, value, size);
    }

    template <typename ObjectsIter,
              typename ObjectSizeFunctor,
              typename ObjectValueFunctor,
              typename Is_0_1_Tag>
    FunctorOnIteratorPValue<ObjectValueFunctor, ObjectsIter>
    getValueUpperBound(ObjectsIter oBegin, ObjectsIter oEnd,
     FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter> capacity, //capacity is of size type
     ObjectValueFunctor value, ObjectSizeFunctor size, ArithmeticSizeTag, Is_0_1_Tag is_0_1_Tag) {
         auto out = boost::make_function_output_iterator(utils::SkipFunctor());

         return std::min(2 * knapsack_general_two_app(oBegin, oEnd, capacity, out, value, size, is_0_1_Tag).first,
                         getDensityBasedValueUpperBound(oBegin, oEnd, capacity, value, size));
    }

    template <typename ObjectsIter,
              typename ObjectSizeFunctor,
              typename ObjectValueFunctor,
              typename Is_0_1_Tag>
    FunctorOnIteratorPValue<ObjectValueFunctor, ObjectsIter>
    getValueUpperBound(ObjectsIter oBegin, ObjectsIter oEnd,
     FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter> capacity, //capacity is of size type
     ObjectValueFunctor value, ObjectSizeFunctor size, Is_0_1_Tag is_0_1_Tag) {
         typedef FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter> SizeType;
         return getValueUpperBound(oBegin, oEnd, capacity, value, size, GetArithmeticSizeTag<SizeType>(), is_0_1_Tag);
    }

}//detail
}//paal
#endif /* GET_UPPER_BOUND_HPP */
