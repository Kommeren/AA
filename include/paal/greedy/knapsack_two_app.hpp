/**
 * @file knapsack_two_app.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-10-07
 */
#ifndef KNAPSACK_TWO_APP_HPP
#define KNAPSACK_TWO_APP_HPP 

#include <type_traits>
#include <utility>

#include <boost/iterator/counting_iterator.hpp>
#include <boost/iterator/filter_iterator.hpp>

#include "paal/utils/knapsack_utils.hpp"
#include "paal/utils/functors.hpp"
#include "paal/utils/type_functions.hpp"

namespace paal {

template <typename OutputIterator, 
          typename ObjectsIter, 
          typename ObjectSizeFunctor, 
          typename ObjectValueFunctor>
std::pair<detail::FunctorOnIteratorPValue<ObjectValueFunctor, ObjectsIter>,
          detail::FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter>>
knapsack_two_app(ObjectsIter oBegin, 
        ObjectsIter oEnd, 
        detail::FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter> capacity,
        OutputIterator out, 
        ObjectValueFunctor value, 
        ObjectSizeFunctor size) {
    typedef detail::FunctorOnIteratorPValue<ObjectValueFunctor, ObjectsIter> ValueType;
    typedef detail::FunctorOnIteratorPValue<ObjectValueFunctor, ObjectsIter> SizeType;

    static_assert(std::is_arithmetic<ValueType>::value &&
                  std::is_arithmetic<SizeType>::value, 
                  "SizeType and Value type must be arithmetic types");

    auto starValue = [=](ObjectsIter oi){return value(*oi);};
    auto starSize = [=](ObjectsIter oi){return size(*oi);};
    auto density = make_Density(starValue, starSize);
    auto compare = utils::make_FunctorToComparator(density);
    std::vector<ObjectsIter> objects(
            boost::make_counting_iterator(oBegin), 
            boost::make_counting_iterator(oEnd));
    
    if(oBegin == oEnd) {
        return std::pair<ValueType, SizeType>();
    }

    //removing to big elements
    objects.erase(
        std::remove_if(objects.begin(), objects.end(), 
            [=](ObjectsIter objIter){return size(*objIter) > capacity;}), 
        objects.end());

    //finding the element with the greatest density
    auto mostDenseIter = std::max_element(objects.begin(), objects.end(), compare);

    auto nr = capacity / size(**mostDenseIter);
    ValueType valueSum = nr * value(**mostDenseIter);
    SizeType sizeSum = nr * size(**mostDenseIter);

    //finding the biggest set elements with the greatest density
    //this is actually small optimization compare to original algorithm
    auto largest = std::max_element(objects.begin(), objects.end(), utils::make_FunctorToComparator(starValue));

    if(largest != objects.end() && value(**largest) > valueSum) {
        *out = **largest;
        return std::make_pair(value(**largest), size(**largest));
    } else {
        for(auto i = 0; i < nr; ++i) {
            *out = **mostDenseIter;
            ++out;
        }
        return std::make_pair(valueSum, sizeSum);
    }
}

}
#endif /* KNAPSACK_TWO_APP_HPP */
