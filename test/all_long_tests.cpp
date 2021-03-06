//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file all_long_tests.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-11-18
 */

#define BOOST_TEST_MODULE all_long_tests

#include "paal/lp/glp.hpp"

#include <boost/test/unit_test.hpp>

#include <iostream>
#include <iomanip>

namespace {
struct Config {
    Config() { std::cout << std::setprecision(20); }
    ~Config() { paal::lp::glp::free_env(); }
};
}

BOOST_GLOBAL_FIXTURE(Config);
