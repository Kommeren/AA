/**
 * @file knapsack.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-09-20
 */
#ifndef KNAPSACK_HPP
#define KNAPSACK_HPP

#include "paal/utils/functors.hpp"
#include "paal/utils/type_functions.hpp"
#include "paal/utils/less_pointees.hpp"
#include "paal/utils/knapsack_utils.hpp"
#include "paal/dynamic/knapsack/fill_knapsack_dynamic_table.hpp"
#include "paal/dynamic/knapsack/get_upper_bound.hpp"
#include "paal/dynamic/knapsack/knapsack_common.hpp"
#include "paal/greedy/knapsack_two_app.hpp"

#include <boost/range/adaptor/reversed.hpp>
#include <boost/optional.hpp>
#include <boost/range/irange.hpp>

#include <vector>

namespace paal {

namespace detail {
/**
 * @brief For knapsack dynamic algorithm for given element the table has to be
 * traversed from the lowest to highest element
 */
struct knapsack_get_position_range {
    template <typename T>
    auto operator()(T begin, T end)->decltype(boost::irange(begin, end)) {
        return boost::irange(begin, end);
    }
};

template <typename ObjectsIter, typename ObjectSizeFunctor,
          typename ObjectValueFunctor, typename OutputIterator,
          typename GetBestElement, typename ValuesComparator>
typename knapsack_base<ObjectsIter, ObjectSizeFunctor,
                       ObjectValueFunctor>::return_type
knapsack_dynamic(
    ObjectsIter oBegin, ObjectsIter oEnd,
    detail::FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter> maxSize,
    OutputIterator out, ObjectSizeFunctor size, ObjectValueFunctor value,
    GetBestElement getBest, ValuesComparator compareValues) {
    using base =
        knapsack_base<ObjectsIter, ObjectSizeFunctor, ObjectValueFunctor>;
    using ObjectRef = typename base::ObjectRef;
    using SizeType = typename base::SizeType;
    using ValueType = typename base::ValueType;
    using ObjIterWithValueOrNull =
        boost::optional<std::pair<ObjectsIter, ValueType>>;

    std::vector<ObjIterWithValueOrNull> objectOnSize(maxSize + 1);

    auto compare = [ = ](const ObjIterWithValueOrNull & left,
                         const ObjIterWithValueOrNull & right) {
        return compareValues(left->second, right->second);
    };

    auto objectOnSizeBegin = objectOnSize.begin();
    auto objectOnSizeEnd = objectOnSize.end();
    fill_knapsack_dynamic_table(objectOnSizeBegin, objectOnSizeEnd, oBegin,
                                oEnd, size,
                                [&](ObjIterWithValueOrNull val, ObjectsIter obj)
                                    ->ObjIterWithValueOrNull{
        return std::make_pair(obj, val->second + value(*obj));
    },
                                compare, [](ObjIterWithValueOrNull & val) {
        val = std::make_pair(ObjectsIter(), ValueType());
    },
                                detail::knapsack_get_position_range());

    // getting position of the max value in the objectOnSize array
    auto maxPos = getBest(objectOnSizeBegin, objectOnSizeEnd, compare);

    // setting solution
    auto remainingSpaceInKnapsack = maxPos;
    while (remainingSpaceInKnapsack != objectOnSizeBegin) {
        assert(*remainingSpaceInKnapsack);
        ObjectRef obj = *((*remainingSpaceInKnapsack)->first);
        *out = obj;
        ++out;
        remainingSpaceInKnapsack -= size(obj);
    }

    typedef std::pair<ValueType, SizeType> return_type;

    // returning result
    if (maxPos != objectOnSizeEnd) {
        assert(*maxPos);
        return return_type((*maxPos)->second, maxPos - objectOnSizeBegin);
    } else {
        return return_type(ValueType(), SizeType());
    }
}

/**
 * @brief Solution to the knapsack problem
 *
 * @tparam OutputIterator
 * @param oBegin given objects
 * @param oEnd
 * @param out the result is returned using output iterator
 * @param size functor that for given object returns its size
 * @param value functor that for given object returns its value
 */
template <typename OutputIterator, typename ObjectsIter,
          typename ObjectSizeFunctor, typename ObjectValueFunctor>
typename detail::knapsack_base<ObjectsIter, ObjectSizeFunctor,
                               ObjectValueFunctor>::return_type
knapsack(ObjectsIter oBegin, ObjectsIter oEnd,
         detail::FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter>
             capacity, // capacity is of size type
         OutputIterator out, ObjectSizeFunctor size, ObjectValueFunctor value,
         no_zero_one_tag, integral_value_tag, retrieve_solution_tag) {
    using base = detail::knapsack_base<ObjectsIter, ObjectSizeFunctor,
                                       ObjectValueFunctor>;
    using ValueType = typename base::ValueType;
    using SizeType = typename base::SizeType;
    using ReturnType = typename base::return_type;
    using TableElementType = boost::optional<std::pair<ObjectsIter, ValueType>>;
    if (oBegin == oEnd) {
        return ReturnType{};
    }
    auto maxSize = get_value_upper_bound(oBegin, oEnd, capacity, value, size,
                                         no_zero_one_tag());
    auto ret = knapsack_dynamic(
        oBegin, oEnd, maxSize, out, value, size,
        get_max_element_on_value_indexed_collection<TableElementType, SizeType>(
            TableElementType(std::make_pair(ObjectsIter(), capacity + 1))),
        utils::Greater{});
    return std::make_pair(ret.second, ret.first);
}

/**
 * @brief Solution to the knapsack problem
 *
 * @tparam OutputIterator
 * @param oBegin given objects
 * @param oEnd
 * @param out the result is returned using output iterator
 * @param size functor that for given object returns its size
 * @param value functor that for given object returns its value
 */
template <typename ObjectsIter, typename OutputIterator,
          typename ObjectSizeFunctor, typename ObjectValueFunctor>
typename detail::knapsack_base<ObjectsIter, ObjectSizeFunctor,
                               ObjectValueFunctor>::return_type
knapsack(ObjectsIter oBegin, ObjectsIter oEnd,
         detail::FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter>
             capacity, // capacity is of size type
         OutputIterator out, ObjectSizeFunctor size, ObjectValueFunctor value,
         no_zero_one_tag, integral_size_tag, retrieve_solution_tag) {
    typedef detail::FunctorOnIteratorPValue<ObjectValueFunctor, ObjectsIter>
        ValueType;
    return knapsack_dynamic(
        oBegin, oEnd, capacity, out, size, value,
        detail::get_max_element_on_capacity_indexed_collection<ValueType>(),
        utils::Less{});
}

} // detail

/**
 * @brief Solution to the knapsack problem
 *
 * @tparam ObjectsIter
 * @tparam OutputIterator
 * @tparam ObjectSizeFunctor
 * @tparam ObjectValueFunctor
 * @param oBegin given objects
 * @param oEnd
 * @param out the result is returned using output iterator
 * @param size functor that for given object returns its size
 * @param value functor that for given object returns its value
 */
template <typename ObjectsIter, typename OutputIterator,
          typename ObjectSizeFunctor,
          typename ObjectValueFunctor = utils::return_something_functor<int, 1>>
std::pair<detail::FunctorOnIteratorPValue<ObjectValueFunctor, ObjectsIter>,
          detail::FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter>>
knapsack(ObjectsIter oBegin, ObjectsIter oEnd,
         detail::FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter>
             capacity, // capacity is of size type
         OutputIterator out, ObjectSizeFunctor size,
         ObjectValueFunctor value = ObjectValueFunctor()) {
    return detail::knapsack_check_integrality(oBegin, oEnd, capacity, out, size,
                                              value, detail::no_zero_one_tag(),
                                              detail::retrieve_solution_tag());
}

} // paal

#endif /* KNAPSACK_HPP */
