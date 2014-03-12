/**
 * @file type_functions.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */
#ifndef TYPE_FUNCTIONS_HPP
#define TYPE_FUNCTIONS_HPP

#include <type_traits>
#include <utility>
#include <tuple>
#include <boost/range.hpp>


namespace paal {
namespace utils {

///for given expression returns its type with removed const and reference
#define puretype(t)  typename std::decay<decltype(t)>::type

///for given collection returns type of its reference
template <typename Collection> struct CollectionToRef {
   typedef typename std::iterator_traits<typename boost::range_iterator<Collection>::type>::reference type;
};

///for given collection returns type of its element
template <typename Collection> struct CollectionToElem {
  typedef typename std::iterator_traits<typename boost::range_iterator<Collection>::type>::value_type type;
};

///returns tuple consisting of k times type T
template <typename T, int k> struct kTuple {
    typedef decltype(std::tuple_cat(std::declval<std::tuple<T>>(),
                                    std::declval<typename  kTuple<T, k-1>::type>())) type;
};

///returns tuple consisting of k times type T; boundary case
template <typename T> struct kTuple<T, 1> {
    typedef std::tuple<T> type;
};

///return type of the function //TODO redundant with std::result_of
template <typename T, typename F, typename... Args>
struct ReturnType {
    typedef  decltype(((std::declval<T*>())->*(std::declval<F>()))(std::declval<Args>()...)) type;
};


///return pure type of function (decays const and reference)
template <class F>
struct PureResultOf {
    typedef typename std::decay<typename std::result_of<F>::type >::type type;
};

} //!utils
} //!paal

#endif //TYPE_FUNCTIONS_HPP
