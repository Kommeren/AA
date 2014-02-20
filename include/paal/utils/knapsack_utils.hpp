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

    /**
     * @brief density functor, for given value and size
     *
     * @tparam Value
     * @tparam Size
     */
template <typename Value, typename Size>
struct Density {

    /**
     * @brief constructor
     *
     * @param value
     * @param size
     */
    Density(Value value, Size size) : m_value(value), m_size(size) {}

    /**
     * @brief operator()
     *
     * @tparam ObjectRef
     * @param obj
     *
     * @return
     */
    template <typename ObjectRef>
    double operator()(ObjectRef obj) const {
        return double(m_value(obj)) / double(m_size(obj));
    }
private:
    Value m_value;
    Size m_size;
};

/**
 * @brief make for Density
 *
 * @tparam Value
 * @tparam Size
 * @param value
 * @param size
 *
 * @return
 */
template <typename Value, typename Size>
Density<Value, Size>
make_Density(Value value, Size size) {
    return Density<Value, Size>(value, size);
}


/**
 * @brief functor checking if given objIter has size smaller than capacity
 *
 * @tparam ObjectSizeFunctor
 * @tparam SizeType
 */
template <typename ObjectSizeFunctor, typename SizeType>
struct RightSize {
    /**
     * @brief constructor
     *
     * @param size
     * @param capacity
     */
    RightSize(ObjectSizeFunctor size, SizeType capacity) :
        m_capacity(capacity), m_size(size) {}

    /**
     * @brief operator()
     *
     * @tparam ObjectsIter
     * @param objIter
     *
     * @return
     */
    template <typename ObjectsIter>
    bool operator()(ObjectsIter objIter) const {
        return m_size(*objIter) <= m_capacity;
    }

private:
    SizeType m_capacity;
    ObjectSizeFunctor m_size;
};

/**
 * @brief make for RightSize
 *
 * @tparam ObjectSizeFunctor
 * @tparam SizeType
 * @param size
 * @param capacity
 *
 * @return
 */
template <typename ObjectSizeFunctor, typename SizeType>
RightSize<ObjectSizeFunctor, SizeType>
make_RightSize(ObjectSizeFunctor size, SizeType capacity) {
    return RightSize<ObjectSizeFunctor, SizeType>(size, capacity);
}


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
} //!detail

/**
 * @brief checks if element size is greater than zero
 *
 * @tparam ObjectSizeFunctor
 */
template <typename ObjectSizeFunctor>
struct NotZeroSize {
    /**
     * @brief constructor
     *
     * @param size
     */
    NotZeroSize(ObjectSizeFunctor size) :
        m_size(std::move(size)) {}

    /**
     * @brief operator()
     *
     * @tparam Object
     * @param o
     *
     * @return
     */
    template <typename Object>
    bool operator()(Object && o) const {
        typedef typename utils::PureResultOf<ObjectSizeFunctor(decltype(o))>::type SizeType;
        return m_size(o) > SizeType();
    }
private:
    ObjectSizeFunctor m_size;
};


/**
 * @brief upper bound is computed as biggest density times capacity +
 *        values for all elements with size 0. It is correct upper bound for 0/1.
 *        For unbounded case there will be no elements with size 0.
 *
 * @tparam ObjectsIter
 * @tparam ObjectSizeFunctor
 * @tparam ObjectValueFunctor
 * @param oBegin
 * @param oEnd
 * @param capacity
 * @param value
 * @param size
 *
 * @return
 */
template <typename ObjectsIter,
           typename ObjectSizeFunctor,
           typename ObjectValueFunctor>
 detail::FunctorOnIteratorPValue<ObjectValueFunctor, ObjectsIter>
 getDensityBasedValueUpperBound(ObjectsIter oBegin, ObjectsIter oEnd,
  detail::FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter> capacity, //capacity is of size type
  ObjectValueFunctor value, ObjectSizeFunctor size) {
      typedef detail::FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter> SizeType;
      typedef typename std::iterator_traits<ObjectsIter>::reference ObjectRef;
      auto density = make_Density(value, size);

      //this filters are really needed only in 0/1 case
      //in not 0/1 case, there is a guarantee that sizes are not 0
      NotZeroSize<ObjectSizeFunctor> notZeroSize(size);
      auto zeroSize = utils::make_NotFunctor(notZeroSize);

      auto b = boost::make_filter_iterator(notZeroSize, oBegin, oEnd);
      auto e = boost::make_filter_iterator(notZeroSize, oEnd, oEnd);

      auto nb = boost::make_filter_iterator(zeroSize, oBegin, oEnd);
      auto ne = boost::make_filter_iterator(zeroSize, oEnd, oEnd);

      auto maxElement = density(*std::max_element(b, e, utils::make_FunctorToComparator(density)));
      return capacity * maxElement +
          std::accumulate(nb, ne, SizeType(), [=](SizeType s, ObjectRef o){return s + value(o);});
}

/**
 * @brief for given range, this functions filter all elements that are larger than capacity
 *
 * @tparam ObjectsIter
 * @tparam ObjectSizeFunctor
 * @tparam ObjectValueFunctor
 * @param oBegin
 * @param oEnd
 * @param capacity
 * @param value
 * @param size
 *
 * @return
 */
template <typename ObjectsIter,
           typename ObjectSizeFunctor,
           typename ObjectValueFunctor>
std::pair<detail::FilteredSizesIterator<ObjectsIter, ObjectSizeFunctor>,
          detail::FilteredSizesIterator<ObjectsIter, ObjectSizeFunctor>>
 filterToLarge(ObjectsIter oBegin, ObjectsIter oEnd,
  detail::FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter> capacity,
  ObjectValueFunctor value, ObjectSizeFunctor size) {
      auto rightSize = make_RightSize(size, capacity);
      auto begin = boost::make_filter_iterator(rightSize, oBegin, oEnd);
      auto end =   boost::make_filter_iterator(rightSize, oEnd, oEnd);
      return std::make_pair(begin, end);
}

/**
 * @brief computes lower bound as  value of the most valuable element
 *
 * @tparam ObjectsIter
 * @tparam ObjectSizeFunctor
 * @tparam ObjectValueFunctor
 * @param oBegin
 * @param oEnd
 * @param value
 * @param size
 *
 * @return
 */
template <typename ObjectsIter,
           typename ObjectSizeFunctor,
           typename ObjectValueFunctor>
 detail::FunctorOnIteratorPValue<ObjectValueFunctor, ObjectsIter>
 getTrivalValueLowerBound(ObjectsIter oBegin, ObjectsIter oEnd,
  ObjectValueFunctor value, ObjectSizeFunctor size) {
      return *std::max_element(oBegin, oEnd, utils::make_FunctorToComparator(value));
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

//various tags
struct IntegralValueAndSizeTag {};
struct IntegralValueTag {};
struct IntegralSizeTag {};
struct NonIntegralValueAndSizeTag {};

struct ArithmeticSizeTag {};
struct NonArithmeticSizeTag {};

struct ZeroOneTag {};
struct NoZeroOneTag {};

struct RetrieveSolutionTag{};
struct NoRetrieveSolutionTag{};

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


template <typename SizeType>
using GetArithmeticSizeTag =
        typename boost::mpl::if_c<std::is_arithmetic<SizeType>::value,
                                 ArithmeticSizeTag,
                                 NonArithmeticSizeTag>::type;

}//!detail

/**
 * @brief computes multiplier for FPTAS, version for 0/1
 *
 * @tparam ObjectsIter
 * @tparam Functor
 * @param oBegin
 * @param oEnd
 * @param epsilon
 * @param lowerBound
 * @param Functor
 * @param detail::ZeroOneTag
 *
 * @return
 */
template <typename ObjectsIter, typename Functor>
boost::optional<double> getMultiplier(ObjectsIter oBegin, ObjectsIter oEnd,
                     double epsilon, double lowerBound, Functor, detail::ZeroOneTag) {
    double n = std::distance(oBegin, oEnd);
    auto ret =  n / (epsilon * lowerBound);
    static const double SMALLEST_MULTIPLIER = 1.;
    if(ret > SMALLEST_MULTIPLIER )
        return boost::optional<double>();
    return  ret;
}


//TODO this multiplier does not guarantee fptas
/**
 * @brief computes multiplier for FPTAS, unbounded version
 *
 * @tparam ObjectsIter
 * @tparam Functor
 * @param oBegin
 * @param oEnd
 * @param epsilon
 * @param lowerBound
 * @param f
 * @param detail::NoZeroOneTag
 *
 * @return
 */
template <typename ObjectsIter, typename Functor>
boost::optional<double> getMultiplier(ObjectsIter oBegin, ObjectsIter oEnd,
                     double epsilon, double lowerBound, Functor f, detail::NoZeroOneTag) {
    double minF = f(*std::min_element(oBegin, oEnd, utils::make_FunctorToComparator(f)));
    double n = int(double(lowerBound) * (1. + epsilon) / minF + 1.); //maximal number of elements in the found solution
    auto ret =  n / (epsilon * lowerBound);
    static const double SMALLEST_MULTIPLIER = 1.;
    if(ret > SMALLEST_MULTIPLIER )
        return boost::optional<double>();
    return  ret;
}

}//paal
#endif /* KNAPSACK_UTILS_HPP */
