/**
 * @file knapsack_utils.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-10-07
 */
#ifndef KNAPSACK_UTILS_HPP
#define KNAPSACK_UTILS_HPP 

#include <boost/iterator/filter_iterator.hpp>
#include <boost/optional.hpp>

#include "paal/utils/type_functions.hpp"
#include "paal/utils/functors.hpp"

namespace paal {

template <typename Value, typename Size>
struct Density {

    Density(Value value, Size size) : m_value(value), m_size(size) {}

    template <typename ObjectRef>
    double operator()(ObjectRef obj) const {
        return double(m_value(obj)) / double(m_size(obj));
    }
private:
    Value m_value;
    Size m_size;
};

template <typename Value, typename Size>
Density<Value, Size>
make_Density(Value value, Size size) {
    return Density<Value, Size>(value, size);
}
    

template <typename ObjectSizeFunctor, typename SizeType>
struct RightSize {
    RightSize(ObjectSizeFunctor size, SizeType capacity) : 
        m_capacity(capacity), m_size(size) {}

    template <typename ObjectsIter>
    bool operator()(ObjectsIter objIter) {
        return m_size(*objIter) <= m_capacity;
    }

private:
    SizeType m_capacity;
    ObjectSizeFunctor m_size;

};

namespace detail {
template <typename Functor, typename Iter>
    using FunctorOnIteratorPValue = 
      puretype(std::declval<Functor>()(*std::declval<Iter>()));

template <typename FunctorLeft, typename FunctorRight, typename Iter>
    using FunctorsOnIteratorPValuePair =
            std::pair<FunctorOnIteratorPValue<FunctorLeft, Iter>,
                      FunctorOnIteratorPValue<FunctorRight, Iter>>;

template <typename ObjectsIter,
          typename ObjectSizeFunctor> 
    using FilteredSizesIterator = 
             boost::filter_iterator<ObjectsIter, 
                  RightSize<ObjectSizeFunctor, FunctorOnIteratorPValue<ObjectsIter, ObjectSizeFunctor>>>;
}

template <typename ObjectsIter,
           typename ObjectSizeFunctor, 
           typename ObjectValueFunctor>
 detail::FunctorOnIteratorPValue<ObjectValueFunctor, ObjectsIter>
 getDensityBasedValueUpperBound(ObjectsIter oBegin, ObjectsIter oEnd, 
  detail::FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter> capacity, //capacity is of size type
  ObjectValueFunctor value, ObjectSizeFunctor size) {
      typedef typename std::iterator_traits<ObjectsIter>::reference  ObjectRef;

      auto density = make_Density(value, size);
      auto maxElement = density(*std::max_element(oBegin, oEnd, utils::make_FunctorToComparator(density)));
      return capacity * maxElement;
}

template <typename ObjectsIter,
           typename ObjectSizeFunctor, 
           typename ObjectValueFunctor>
std::pair<detail::FilteredSizesIterator<ObjectsIter, ObjectSizeFunctor>,
          detail::FilteredSizesIterator<ObjectsIter, ObjectSizeFunctor>>
 filterToLarge(ObjectsIter oBegin, ObjectsIter oEnd, 
  detail::FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter> capacity,
  ObjectValueFunctor value, ObjectSizeFunctor size) {
      typedef detail::FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter> SizeType;
      RightSize<ObjectSizeFunctor, SizeType> rightSize(size, capacity);
      auto begin = boost::make_filter_iterator(rightSize, oBegin, oEnd);
      auto end =   boost::make_filter_iterator(rightSize, oEnd, oEnd);
      return std::make_pair(begin, end);
}

template <typename ObjectsIter,
           typename ObjectSizeFunctor, 
           typename ObjectValueFunctor>
 detail::FunctorOnIteratorPValue<ObjectValueFunctor, ObjectsIter>
 getTrivalValueLowerBound(ObjectsIter oBegin, ObjectsIter oEnd, 
  ObjectValueFunctor value, ObjectSizeFunctor size) {
      typedef typename std::iterator_traits<ObjectsIter>::reference  ObjectRef;
      auto density = make_Density(value, size);
      return *std::max_element(oBegin, oEnd, utils::make_FunctorToComparator(density));
}
   

namespace detail {
//definition of basic types
template <typename ObjectsIter, 
         typename ObjectSizeFunctor, 
         typename ObjectValueFunctor>
struct KnapsackBase {
    typedef detail::FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter> SizeType;
    typedef detail::FunctorOnIteratorPValue<ObjectValueFunctor, ObjectsIter> ValueType;
    typedef puretype(*std::declval<ObjectsIter>()) ObjectType;
    typedef typename std::iterator_traits<ObjectsIter>::reference ObjectRef;
    typedef std::pair<ValueType, SizeType> ReturnType;
};

//if the knapsack dynamic table is indexed by values,
//the procedure to find the best element is to find the biggest index i in the table that
// *i is smaller than given threshold(capacity)
template <typename MaxValueType, typename SizeType>
struct GetMaxElementOnValueIndexedCollection {
    GetMaxElementOnValueIndexedCollection(MaxValueType maxValue) :
        m_maxValue(maxValue) {}

    template <typename Iterator, typename Comparator>
        Iterator operator()(Iterator begin, Iterator end, Comparator compare) {
            typedef std::pair<Iterator, SizeType> Return;
            auto compareOpt = make_less_pointees_t(compare);
            //traverse in reverse order, skip the first
            for(auto iter = end - 1; iter != begin; --iter ) {
                if(*iter && compareOpt(m_maxValue, *iter)) {
                    return iter;
                }
            }

            return end;
        }
private:
    MaxValueType m_maxValue;
};

//if the knapsack dynamic table is indexed by sizes,
//the procedure to find the best element is to find the biggest 
//index i in the table that maximizes *i
template <typename ValueType>
struct GetMaxElementOnCapacityIndexedCollection {
    template <typename Iterator, typename Comparator>
        Iterator operator()(Iterator begin, Iterator end, Comparator compare) {
            return std::max_element(begin, end, make_less_pointees_t(compare));
        }
};

struct IntegralValueAndSizeTag {};
struct IntegralValueTag {};
struct IntegralSizeTag {};
struct NonIntegralValueAndSizeTag {};

template <typename SizeType, typename ValueType>
using GetIntegralTag =  
        typename boost::mpl::if_c<std::is_integral<SizeType>::value && 
                       std::is_integral<ValueType>::value, IntegralValueAndSizeTag, 
                            typename boost::mpl::if_c<std::is_integral<SizeType>::value, IntegralSizeTag,
                                typename boost::mpl::if_c<std::is_integral<ValueType>::value, 
                                                          IntegralValueTag,
                                                          NonIntegralValueAndSizeTag
                                                         >::type
                                                      >::type
                      >::type;

struct ArithmeticSizeTag {};
struct NonArithmeticSizeTag {};

template <typename SizeType>
using GetArithmeticSizeTag =  
        typename boost::mpl::if_c<std::is_arithmetic<SizeType>::value, 
                                 ArithmeticSizeTag,
                                 NonArithmeticSizeTag>::type;

}//detail

template <typename ObjectsIter>
boost::optional<double> getMultiplier(ObjectsIter oBegin, ObjectsIter oEnd, 
                     double epsilon, double lowerBound) {
    double n = std::distance(oBegin, oEnd);
    auto ret =  n / (epsilon * lowerBound);
    static const double SMALLEST_MULTIPLIER = 1.;
    if(ret > SMALLEST_MULTIPLIER )
        return boost::optional<double>();
    return  ret;
}

}//paal
#endif /* KNAPSACK_UTILS_HPP */
