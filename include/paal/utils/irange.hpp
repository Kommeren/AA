/**
 * @file irange.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2014-10-3
 */
#ifndef IRANGE_HPP
#define IRANGE_HPP

#include <boost/range/irange.hpp>

namespace paal {

template <typename T>
auto irange(T begin, T end) {
    return boost::irange(begin, end);
}

template <typename T>
auto irange(T end) {
    return irange(T{}, end);
}

}

#endif /* IRANGE_HPP */
