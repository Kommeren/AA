/**
 * @file knapsack_utils.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-10-07
 */
#ifndef KNAPSACK_UTILS_HPP
#define KNAPSACK_UTILS_HPP


#include "paal/utils/type_functions.hpp"
#include "paal/utils/less_pointees.hpp"
#include "paal/utils/functors.hpp"

#include <boost/iterator/filter_iterator.hpp>
#include <boost/optional.hpp>


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
struct right_size {
    /**
     * @brief constructor
     *
     * @param size
     * @param capacity
     */
    right_size(ObjectSizeFunctor size, SizeType capacity) :
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
 * @brief make for right_size
 *
 * @tparam ObjectSizeFunctor
 * @tparam SizeType
 * @param size
 * @param capacity
 *
 * @return
 */
template <typename ObjectSizeFunctor, typename SizeType>
right_size<ObjectSizeFunctor, SizeType>
make_right_size(ObjectSizeFunctor size, SizeType capacity) {
    return right_size<ObjectSizeFunctor, SizeType>(size, capacity);
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
                  right_size<ObjectSizeFunctor, FunctorOnIteratorPValue<ObjectsIter, ObjectSizeFunctor>>>;
} //!detail

/**
 * @brief checks if element size is greater than zero
 *
 * @tparam ObjectSizeFunctor
 */
template <typename ObjectSizeFunctor>
struct not_zero_size {
    /**
     * @brief constructor
     *
     * @param size
     */
    not_zero_size(ObjectSizeFunctor size) :
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
        typedef typename utils::pure_result_of<ObjectSizeFunctor(decltype(o))>::type SizeType;
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
 get_density_based_value_upper_bound(ObjectsIter oBegin, ObjectsIter oEnd,
  detail::FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter> capacity, //capacity is of size type
  ObjectValueFunctor value, ObjectSizeFunctor size) {
      typedef detail::FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter> SizeType;
      typedef typename std::iterator_traits<ObjectsIter>::reference ObjectRef;
      auto density = make_Density(value, size);

      //this filters are really needed only in 0/1 case
      //in not 0/1 case, there is a guarantee that sizes are not 0
      not_zero_size<ObjectSizeFunctor> notZeroSize(size);
      auto zeroSize = utils::make_not_functor(notZeroSize);

      auto b = boost::make_filter_iterator(notZeroSize, oBegin, oEnd);
      auto e = boost::make_filter_iterator(notZeroSize, oEnd, oEnd);

      auto nb = boost::make_filter_iterator(zeroSize, oBegin, oEnd);
      auto ne = boost::make_filter_iterator(zeroSize, oEnd, oEnd);

      auto maxElement = density(*std::max_element(b, e, utils::make_functor_to_comparator(density)));
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
 filter_to_large(ObjectsIter oBegin, ObjectsIter oEnd,
  detail::FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter> capacity,
  ObjectValueFunctor value, ObjectSizeFunctor size) {
      auto rightSize = make_right_size(size, capacity);
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
 get_trival_value_lower_bound(ObjectsIter oBegin, ObjectsIter oEnd,
  ObjectValueFunctor value, ObjectSizeFunctor size) {
      return *std::max_element(oBegin, oEnd, utils::make_functor_to_comparator(value));
}


namespace detail {
//definition of basic types
template <typename ObjectsIter,
         typename ObjectSizeFunctor,
         typename ObjectValueFunctor>
struct knapsack_base {
    typedef detail::FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter> SizeType;
    typedef detail::FunctorOnIteratorPValue<ObjectValueFunctor, ObjectsIter> ValueType;
    typedef puretype(*std::declval<ObjectsIter>()) ObjectType;
    typedef typename std::iterator_traits<ObjectsIter>::reference ObjectRef;
    typedef std::pair<ValueType, SizeType> return_type;
};

//if the knapsack dynamic table is indexed by values,
//the procedure to find the best element is to find the biggest index i in the table that
// *i is smaller than given threshold(capacity)
template <typename MaxValueType, typename SizeType>
struct get_max_element_on_value_indexed_collection {
    get_max_element_on_value_indexed_collection(MaxValueType maxValue) :
        m_max_value(maxValue) {}

    template <typename Iterator, typename Comparator>
        Iterator operator()(Iterator begin, Iterator end, Comparator compare) {
            auto compareOpt = make_less_pointees_t(compare);
            //traverse in reverse order, skip the first
            for(auto iter = end - 1; iter != begin; --iter ) {
                if(*iter && compareOpt(m_max_value, *iter)) {
                    return iter;
                }
            }

            return end;
        }
private:
    MaxValueType m_max_value;
};

//if the knapsack dynamic table is indexed by sizes,
//the procedure to find the best element is to find the biggest
//index i in the table that maximizes *i
template <typename ValueType>
struct get_max_element_on_capacity_indexed_collection {
    template <typename Iterator, typename Comparator>
        Iterator operator()(Iterator begin, Iterator end, Comparator compare) {
            return std::max_element(begin, end, make_less_pointees_t(compare));
        }
};

//various tags
struct integral_value_and_size_tag {};
struct integral_value_tag {};
struct integral_size_tag {};
struct non_integral_value_and_size_tag {};

struct arithmetic_size_tag {};
struct Nonarithmetic_size_tag {};

struct zero_one_tag {};
struct no_zero_one_tag {};

struct retrieve_solution_tag{};
struct no_retrieve_solution_tag{};

template <typename SizeType, typename ValueType>
using GetIntegralTag =
        typename boost::mpl::if_c<std::is_integral<SizeType>::value &&
                       std::is_integral<ValueType>::value, integral_value_and_size_tag,
                            typename boost::mpl::if_c<std::is_integral<SizeType>::value, integral_size_tag,
                                typename boost::mpl::if_c<std::is_integral<ValueType>::value,
                                                          integral_value_tag,
                                                          non_integral_value_and_size_tag
                                                         >::type
                                                      >::type
                      >::type;


template <typename SizeType>
using Getarithmetic_size_tag =
        typename boost::mpl::if_c<std::is_arithmetic<SizeType>::value,
                                 arithmetic_size_tag,
                                 Nonarithmetic_size_tag>::type;

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
 * @param detail::zero_one_tag
 *
 * @return
 */
template <typename ObjectsIter, typename Functor>
boost::optional<double> get_multiplier(ObjectsIter oBegin, ObjectsIter oEnd,
                     double epsilon, double lowerBound, Functor, detail::zero_one_tag) {
    double n = std::distance(oBegin, oEnd);
    auto ret =  n / (epsilon * lowerBound);
    static const double SMALLEST_MULTIPLIER = 1.;
    if(ret > SMALLEST_MULTIPLIER )
        return boost::none;
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
 * @param detail::no_zero_one_tag
 *
 * @return
 */
template <typename ObjectsIter, typename Functor>
boost::optional<double> get_multiplier(ObjectsIter oBegin, ObjectsIter oEnd,
                     double epsilon, double lowerBound, Functor f, detail::no_zero_one_tag) {
    double minF = f(*std::min_element(oBegin, oEnd, utils::make_functor_to_comparator(f)));
    double n = int(double(lowerBound) * (1. + epsilon) / minF + 1.); //maximal number of elements in the found solution
    auto ret =  n / (epsilon * lowerBound);
    static const double SMALLEST_MULTIPLIER = 1.;
    if(ret > SMALLEST_MULTIPLIER )
        return boost::none;
    return  ret;
}

}//paal
#endif /* KNAPSACK_UTILS_HPP */
