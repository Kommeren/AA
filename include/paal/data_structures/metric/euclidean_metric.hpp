//=======================================================================
// Copyright (c) 2013 Piotr Wygocki
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file euclidean_metric.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-10-28
 */
#ifndef EUCLIDEAN_METRIC_HPP
#define EUCLIDEAN_METRIC_HPP

#include "paal/data_structures/metric/metric_traits.hpp"

#include <cmath>

namespace paal {
namespace data_structures {


///metric with euclidean distance
template <typename T>
struct euclidean_metric {
    ///operator()
    int operator()(const std::pair<T, T> &p1,
                   const std::pair<T, T> &p2) const {
        return std::hypot(p1.first - p2.first, p1.second - p2.second);
    }
};

template <typename T> struct metric_traits<euclidean_metric<T>>
: public  _metric_traits<euclidean_metric<T>, std::pair<T, T>> {
};
} // data_structures

} // paal

#endif /* EUCLIDEAN_METRIC_HPP */
