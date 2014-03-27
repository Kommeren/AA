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

    /**
     * @brief this iterator exlcludes one specific element from range
     *
     * @tparam Iterator
     */
template <typename Iterator>
struct iterator_with_excluded_element :
    public boost::filter_iterator<decltype(std::bind(utils::not_equal_to(),
                                           std::declval<typename std::iterator_traits<Iterator>::value_type>() ,
                                           std::placeholders::_1)), Iterator> {

    typedef typename std::iterator_traits<Iterator>::value_type Element;
    /**
     * @brief constructor
     *
     * @param i
     * @param end
     * @param e
     */
    iterator_with_excluded_element(Iterator i, Iterator end, const Element &  e)
        : boost::filter_iterator<decltype(std::bind(utils::not_equal_to(),
                                                    std::declval<Element>(),
                                                    std::placeholders::_1)), Iterator >
          (std::bind(utils::not_equal_to(), e, std::placeholders::_1), i, end )  {}

    iterator_with_excluded_element() = default;
};

} //utils
} //paal

#endif // ITERATOR_HELPERS_HPP

