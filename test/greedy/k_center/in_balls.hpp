//=======================================================================
// Copyright (c) 2014 Piotr Smulewicz
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file in_balls.cpp
 * @brief
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2014-01-29
 */

#ifndef IN_BALLS_HPP
#define IN_BALLS_HPP

#include <boost/test/unit_test.hpp>

namespace paal {

template <typename IT, typename CE, typename Metric, typename Dist>
void in_balls(const IT &items, const CE &centers, const Metric &metric,
              Dist radius) {
    static const auto EPSILON = 0.00001;
    for (auto j : items) {
        bool inBall = false;
        for (auto i : centers) {
            if (metric(j, i) <= radius + EPSILON) {
                inBall = true;
                break;
            }
        }
        if (!inBall) {
            LOGLN("Item " << j << " is not included in any ball");
            ON_LOG(for (auto i
                        : centers) {)
                LOGLN("Distance to " << i << " = " << metric(j, i));
            ON_LOG(
            })
        }
        BOOST_CHECK(inBall);
    }
}
} //! paal
#endif /* IN_BALLS_HPP */
