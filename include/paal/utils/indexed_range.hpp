/**
 * @file indexed_range.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2014-10-3
 */
#ifndef INDEXED_RANGE_HPP
#define INDEXED_RANGE_HPP

#include <boost/range/adaptor/indexed.hpp>
#include <boost/range/counting_range.hpp>

namespace paal {

/**
 * @brief
 *
 * @tparam Range
 * @param r
 * @param start_index
 */
template <typename Range>
auto indexed_range(Range && r, int start_index = 0) {
    return boost::counting_range(r | boost::adaptors::indexed(start_index));
}

} //! paal

#endif /* INDEXED_RANGE_HPP */
