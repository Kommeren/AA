/**
 * @file iterator_helpers.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */

#include <boost/iterator/filter_iterator.hpp>

#ifndef __ITERATOR_HELPERS__
#define __ITERATOR_HELPERS__

namespace paal {
namespace helpers {

template <typename Iterator, typename Element> class IteratorWithExcludedElement : 
    public boost::filter_iterator<std::function<bool(const Element &)>, Iterator> {
public:
    IteratorWithExcludedElement(Iterator i, Iterator end, const Element &  e) 
        : boost::filter_iterator<std::function<bool(const Element &)>, Iterator >
          (std::bind(std::not_equal_to<Element>(), e, std::placeholders::_1), i, end )  {}
    
    IteratorWithExcludedElement() {} 
};

} //helpers
} //paal

#endif // __ITERATOR_HELPERS__

