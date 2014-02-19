/**
 * @file all_small_tests.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-11-18
 */

#define BOOST_TEST_MODULE all_small_tests
#include <boost/test/unit_test.hpp>
#include "paal/lp/glp.hpp"

namespace {
struct Config {
    ~Config()  {paal::lp::GLP::freeEnv(); }
};
}

BOOST_GLOBAL_FIXTURE( Config );
