//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file all_small_tests.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-11-18
 */

#define BOOST_TEST_MODULE all_small_tests

#include "test_utils/get_test_dir.hpp"

#include "paal/lp/glp.hpp"

#include <boost/test/unit_test.hpp>

#include <iostream>
#include <iomanip>
#include <stdlib.h>


namespace {
struct Config {
    Config() {
        std::cout << std::setprecision(20);

        std::string const tmp = get_temp_dir();
        std::system(("rm -fr " + tmp).c_str());
        std::system(("mkdir -v -p " + tmp).c_str());
    }

    ~Config() {
        std::string tmp = get_temp_dir();
        std::system(("rm -fr " + tmp).c_str());

        paal::lp::glp::free_env();
    }


    };
}

BOOST_GLOBAL_FIXTURE(Config);
