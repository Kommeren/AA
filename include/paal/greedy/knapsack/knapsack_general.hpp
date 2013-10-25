
/**
 * @file knapsack_general.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-10-07
 */


#ifndef KNAPSACK_GENERAL_HPP
#define KNAPSACK_GENERAL_HPP 

#include <boost/iterator/counting_iterator.hpp>
#include "paal/utils/knapsack_utils.hpp"

namespace paal {
namespace detail {


struct ZeroOneTag {};
struct NoZeroOneTag {};


template <typename OutputIterator, 
         typename ObjectsIter, 
         typename ObjectSizeFunctor, 
         typename ObjectValueFunctor,
         typename Is_0_1_Tag>
             std::pair<detail::FunctorOnIteratorPValue<ObjectValueFunctor, ObjectsIter>,
         FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter>>
             knapsack_general_two_app(ObjectsIter oBegin, 
                     ObjectsIter oEnd, 
                     detail::FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter> capacity,
                     OutputIterator out, 
                     ObjectValueFunctor value, 
                     ObjectSizeFunctor size,
                     Is_0_1_Tag is_0_1_Tag) 
             {
                 typedef FunctorOnIteratorPValue<ObjectValueFunctor, ObjectsIter> ValueType;
                 typedef FunctorOnIteratorPValue<ObjectValueFunctor, ObjectsIter> SizeType;

                 static_assert(std::is_arithmetic<ValueType>::value &&
                         std::is_arithmetic<SizeType>::value, 
                         "SizeType and Value type must be arithmetic types");

                 auto starValue = [=](ObjectsIter oi){return value(*oi);};
                 std::vector<ObjectsIter> objects(
                         boost::make_counting_iterator(oBegin), 
                         boost::make_counting_iterator(oEnd));

                 //removing to big elements
                 objects.erase(
                         std::remove_if(objects.begin(), objects.end(), 
                             utils::make_NotFunctor(make_RightSize(size, capacity))),
                         objects.end());
                 
                 if(objects.empty()) {
                     return std::pair<ValueType, SizeType>();
                 }

                 //finding the element with the greatest density
                 auto greedyFill = getGreedyFill(objects.begin(), objects.end(), capacity, value, size, is_0_1_Tag);

                 //finding the biggest set elements with the greatest density
                 //this is actually small optimization compare to original algorithm
                 auto largest = std::max_element(objects.begin(), objects.end(), utils::make_FunctorToComparator(starValue));

                 if(value(**largest) > std::get<0>(greedyFill)) {
                     *out = **largest;
                     return std::make_pair(value(**largest), size(**largest));
                 } else {
                     greedyToOutput(std::get<2>(greedyFill), out, is_0_1_Tag);
                     return std::make_pair(std::get<0>(greedyFill), std::get<1>(greedyFill));
                 }
             }
} //detail
} //paal
#endif /* KNAPSACK_GENERAL_HPP */
