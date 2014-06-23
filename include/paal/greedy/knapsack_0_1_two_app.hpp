/**
 * @file knapsack_0_1_two_app.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-10-07
 */
#ifndef KNAPSACK_0_1_TWO_APP_HPP
#define KNAPSACK_0_1_TWO_APP_HPP

#include "paal/utils/knapsack_utils.hpp"
#include "paal/utils/functors.hpp"
#include "paal/utils/type_functions.hpp"
#include "paal/greedy/knapsack/knapsack_general.hpp"

#include <boost/iterator/counting_iterator.hpp>
#include <boost/iterator/filter_iterator.hpp>

#include <type_traits>
#include <utility>

namespace paal {

namespace detail {
template <typename ObjectsIterIter, typename ObjectSizeFunctor,
          typename ObjectValueFunctor>
std::tuple<FunctorOnIteratorPValue<
               ObjectValueFunctor,
               typename std::iterator_traits<ObjectsIterIter>::value_type>,
           FunctorOnIteratorPValue<
               ObjectValueFunctor,
               typename std::iterator_traits<ObjectsIterIter>::value_type>,
           std::pair<ObjectsIterIter, ObjectsIterIter>>
get_greedy_fill(
    ObjectsIterIter oBegin, ObjectsIterIter oEnd,
    FunctorOnIteratorPValue<
        ObjectSizeFunctor,
        typename std::iterator_traits<ObjectsIterIter>::value_type> capacity,
    ObjectValueFunctor value, ObjectSizeFunctor size, zero_one_tag) {
    typedef typename std::iterator_traits<ObjectsIterIter>::value_type
        ObjectsIter;
    typedef FunctorOnIteratorPValue<ObjectValueFunctor, ObjectsIter> ValueType;
    typedef FunctorOnIteratorPValue<ObjectValueFunctor, ObjectsIter> SizeType;

    auto starValue = utils::make_lift_iterator_functor(value);
    auto starSize = utils::make_lift_iterator_functor(size);
    auto density = make_Density(starValue, starSize);
    auto compare = utils::make_functor_to_comparator(density, utils::Greater());

    // finding the biggest set elements with the greatest density
    std::sort(oBegin, oEnd, compare);

    ValueType valueSum = ValueType();
    SizeType sizeSum = SizeType();
    auto end = std::find_if(oBegin, oEnd,
                            [ =, &sizeSum, &valueSum](ObjectsIter objIter) {
        auto newSize = sizeSum + size(*objIter);
        if (newSize > capacity) {
            return true;
        }
        sizeSum = newSize;
        valueSum += value(*objIter);
        return false;
    });
    return std::make_tuple(valueSum, sizeSum, std::make_pair(oBegin, end));
}

template <typename ObjectsRange, typename OutputIter>
void greedy_to_output(ObjectsRange range, OutputIter out, zero_one_tag) {
    for (auto obj : boost::make_iterator_range(range)) {
        *out = *obj;
        ++out;
    }
}

} // detail

template <typename OutputIterator, typename ObjectsIter,
          typename ObjectSizeFunctor, typename ObjectValueFunctor>
typename detail::knapsack_base<ObjectsIter, ObjectSizeFunctor,
                               ObjectValueFunctor>::return_type
knapsack_0_1_two_app(
    ObjectsIter oBegin, ObjectsIter oEnd,
    detail::FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter> capacity,
    OutputIterator out, ObjectValueFunctor value, ObjectSizeFunctor size) {
    return detail::knapsack_general_two_app(oBegin, oEnd, capacity, out, value,
                                            size, detail::zero_one_tag());
}
}
#endif /* KNAPSACK_0_1_TWO_APP_HPP */
