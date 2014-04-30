/**
 * @file get_lower_bound.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-10-04
 */

#ifndef GET_LOWER_BOUND_HPP
#define GET_LOWER_BOUND_HPP


#include "paal/utils/knapsack_utils.hpp"
#include "paal/greedy/knapsack/knapsack_general.hpp"

#include <boost/function_output_iterator.hpp>



namespace paal {
namespace detail {

    template <typename ObjectsIter,
              typename ObjectSizeFunctor,
              typename ObjectValueFunctor,
              typename Is_0_1_Tag>
    FunctorOnIteratorPValue<ObjectValueFunctor, ObjectsIter>
    get_value_lower_bound(ObjectsIter oBegin, ObjectsIter oEnd,
     FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter> capacity, //capacity is of size type
     ObjectValueFunctor value, ObjectSizeFunctor size, Nonarithmetic_size_tag, Is_0_1_Tag) {
         return get_trival_value_lower_bound(oBegin, oEnd, value, size);
    }

    template <typename ObjectsIter,
              typename ObjectSizeFunctor,
              typename ObjectValueFunctor,
              typename Is_0_1_Tag>
    FunctorOnIteratorPValue<ObjectValueFunctor, ObjectsIter>
    get_value_lower_bound(ObjectsIter oBegin, ObjectsIter oEnd,
     FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter> capacity, //capacity is of size type
     ObjectValueFunctor value, ObjectSizeFunctor size, arithmetic_size_tag, Is_0_1_Tag is_0_1_Tag) {
        auto out = boost::make_function_output_iterator(utils::skip_functor());
        return knapsack_general_two_app(oBegin, oEnd, capacity, out, value, size, is_0_1_Tag).first;
    }

    template <typename ObjectsIter,
              typename ObjectSizeFunctor,
              typename ObjectValueFunctor,
              typename Is_0_1_Tag>
    FunctorOnIteratorPValue<ObjectValueFunctor, ObjectsIter>
    get_value_lower_bound(ObjectsIter oBegin, ObjectsIter oEnd,
     FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter> capacity, //capacity is of size type
     ObjectValueFunctor value, ObjectSizeFunctor size, Is_0_1_Tag is_0_1_Tag) {
         typedef FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter> SizeType;
         return get_value_lower_bound(oBegin, oEnd, capacity, value, size, Getarithmetic_size_tag<SizeType>(), is_0_1_Tag);
    }

}//detail
}//paal
#endif /* GET_LOWER_BOUND_HPP */
