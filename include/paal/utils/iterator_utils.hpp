/**
 * @file iterator_utils.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */

#include <boost/iterator/filter_iterator.hpp>
#include <boost/iterator/transform_iterator.hpp>

#include "type_functions.hpp"

#ifndef ITERATOR_HELPERS_HPP
#define ITERATOR_HELPERS_HPP

namespace paal {
namespace utils {

template <typename Iterator> class IteratorWithExcludedElement : 
    public boost::filter_iterator<std::function<bool(const typename IterToElem<Iterator>::type &)>, Iterator> {
public:
    typedef typename IterToElem<Iterator>::type Element;
    IteratorWithExcludedElement(Iterator i, Iterator end, const Element &  e) 
        : boost::filter_iterator<std::function<bool(const Element &)>, Iterator >
          (std::bind(std::not_equal_to<Element>(), e, std::placeholders::_1), i, end )  {}
    
    IteratorWithExcludedElement() {} 
};


//ranges adaptor for pair
template<typename Iter>
struct range : std::pair<Iter, Iter> {
    range( const std::pair<Iter,Iter> & p) : std::pair<Iter, Iter>(p) {}
};

template<typename Iter>
Iter begin(const range<Iter> & p ) {
    return p.first;
}

template<typename Iter>
Iter end(const range<Iter> & p ) {
    return p.second;
}

template<typename T>
range<T> make_range(const std::pair<T, T> & t ) {
    return range<T>(t);
}

template<typename T>
range<T> make_range(T b, T e) {
    return range<T>(std::make_pair(b, e));
}


} //utils
} //paal

#endif // ITERATOR_HELPERS_HPP

