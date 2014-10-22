//=======================================================================
// Copyright (c) Piotr Wygocki
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file assign_update_test.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-06-20
 */
#include <boost/test/unit_test.hpp>

#include "paal/utils/assign_updates.hpp"


BOOST_AUTO_TEST_CASE(max_update_test) {
    int t = 7;

    paal::assign_max(t, 8);
    paal::assign_max(t, 5);

    BOOST_CHECK_EQUAL(t, 8);

    int u = 5;
    paal::assign_max(t, u);
    BOOST_CHECK_EQUAL(t, 8);

    u = 10;
    paal::assign_max(t, u);
    BOOST_CHECK_EQUAL(t, 10);
}

BOOST_AUTO_TEST_CASE(min_update_test) {
    int t = 7;

    paal::assign_min(t, 8);
    paal::assign_min(t, 5);

    BOOST_CHECK_EQUAL(t, 5);

    int u = 10;
    paal::assign_min(t, u);
    BOOST_CHECK_EQUAL(t, 5);

    u = 2;
    paal::assign_min(t, u);
    BOOST_CHECK_EQUAL(t, 2);
}

