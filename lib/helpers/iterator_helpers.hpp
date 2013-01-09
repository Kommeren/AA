/*
 *  * iterator_helpers.hpp
 *  *
 *  *  Created on: 03-01-2013
 *  *      Author: Piotr Wygocki
 *  */

#include <boost/iterator/filter_iterator.hpp>

namespace paal {
namespace helpers {

template <typename Iterator, typename Element> class IteratorWithExcludedElement : 
    public boost::filter_iterator<std::function<bool(Element)>, Iterator> {
public:
    IteratorWithExcludedElement(Iterator i, Iterator end, const Element &  e) 
        : boost::filter_iterator<std::function<bool(Element)>, Iterator >
          (std::bind(std::not_equal_to<Element>(), e, std::placeholders::_1), i, end )  {}
};

} //helpers
} //paal


