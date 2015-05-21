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
#include "test_utils/system.hpp"

#include "paal/lp/glp.hpp"

#include <boost/test/unit_test.hpp>

#include <iostream>
#include <iomanip>
#include <stdlib.h>

namespace {
struct Config {
    Config() {
        std::cout << std::setprecision(20);

        std::string const tmp = paal::system::get_static_temp_dir();
        paal::system::create_directory(tmp);
        std::cout << "Created temp tests directory: " + tmp << std::endl;
    }

    ~Config() {
        std::string tmp = paal::system::get_static_temp_dir();
        paal::system::remove_tmp_path(tmp);

        paal::lp::glp::free_env();
    }


    };
}

BOOST_GLOBAL_FIXTURE(Config);
