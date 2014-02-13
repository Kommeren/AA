//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file fast_exp_test.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-06-20
 */
#include "test_utils/logger.hpp"

#include "paal/utils/fast_exp.hpp"
#include "paal/utils/irange.hpp"
#include "paal/utils/assign_updates.hpp"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(FastExpTest) {
    double biggestErr = 0;
    for (int i : paal::irange(-80, 80)) {
        paal::assign_max(biggestErr, std::abs(exp(i) - paal::fast_exp(i)) / exp(i));
    }
    LOGLN("biggest error " << biggestErr);
    BOOST_CHECK(biggestErr < 0.04);
}
