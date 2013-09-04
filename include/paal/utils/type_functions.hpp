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


namespace paal {
namespace utils {

#define puretype(t)  typename std::decay<decltype(t)>::type 

template <typename Iter> struct IterToElem {
    typedef puretype(*std::declval<Iter>()) type; 
};

template <typename Collection> struct CollectionToIter {
    typedef decltype(std::declval<Collection>().begin()) type; 
};

template <typename Collection> struct CollectionToConstIter {
    typedef decltype(std::declval<const Collection>().begin()) type; 
};


template <typename Collection> struct CollectionToElem {
  typedef typename IterToElem<typename CollectionToIter<Collection>::type>::type type; 
};


template <typename T, int k> struct kTuple {
    typedef decltype(std::tuple_cat(std::declval<std::tuple<T>>(), 
                                    std::declval<typename  kTuple<T, k-1>::type>())) type; 
};

template <typename T> struct kTuple<T, 1> {
    typedef std::tuple<T> type;
};

    
template <typename T, typename F, typename... Args> 
struct ReturnType {
    typedef  decltype(((std::declval<T*>())->*(std::declval<F>()))(std::declval<Args>()...)) type;
};

} //utils
} //paal

#endif //TYPE_FUNCTIONS_HPP
