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
#include "functors.hpp"

#ifndef ITERATOR_HELPERS_HPP
#define ITERATOR_HELPERS_HPP

namespace paal {
namespace utils {

template <typename Iterator>
struct IteratorWithExcludedElement :
    public boost::filter_iterator<decltype(std::bind(utils::NotEqualTo(),
                                           std::declval<typename std::iterator_traits<Iterator>::value_type>() ,
                                           std::placeholders::_1)), Iterator> {

    typedef typename std::iterator_traits<Iterator>::value_type Element;
    IteratorWithExcludedElement(Iterator i, Iterator end, const Element &  e)
        : boost::filter_iterator<decltype(std::bind(utils::NotEqualTo(),
                                                    std::declval<Element>(),
                                                    std::placeholders::_1)), Iterator >
          (std::bind(utils::NotEqualTo(), e, std::placeholders::_1), i, end )  {}

    IteratorWithExcludedElement() {}
};

} //utils
} //paal

#endif // ITERATOR_HELPERS_HPP

