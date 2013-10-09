/**
 * @file iterator_utils.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */
#include <functional>

#include <boost/iterator/filter_iterator.hpp>

#include "type_functions.hpp"

#ifndef ITERATOR_HELPERS_HPP
#define ITERATOR_HELPERS_HPP

namespace paal {
namespace utils {

template <typename Iterator> class IteratorWithExcludedElement : 
    public boost::filter_iterator<std::function<bool(const typename std::iterator_traits<Iterator>::value_type &)>, Iterator> {
public:
    typedef typename std::iterator_traits<Iterator>::value_type Element;
    IteratorWithExcludedElement(Iterator i, Iterator end, const Element &  e) 
        : boost::filter_iterator<std::function<bool(const Element &)>, Iterator >
          (std::bind(std::not_equal_to<Element>(), e, std::placeholders::_1), i, end )  {}
    
    IteratorWithExcludedElement() {} 
};

} //utils
} //paal

#endif // ITERATOR_HELPERS_HPP

