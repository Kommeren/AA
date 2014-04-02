/**
 * @file compilation_error_setup.hpp
 * @brief
 * @author Piotr Godlewski
 * @version 1.0
 * @date 2014-04-29
 */
#ifndef COMPILATION_ERROR_SETUP_HPP
#define COMPILATION_ERROR_SETUP_HPP

#include "paal/lp/glp.hpp"

#ifdef _COMPILATION_ERROR_TEST_
    #define BOOST_TEST_MODULE compilation_error_tests
#endif
#include <boost/test/unit_test.hpp>

namespace {
struct compilation_error_setup {
    compilation_error_setup() : lp("compilation error LP") {
        X = lp.add_column();
        Y = lp.add_column();
        row = lp.add_row(X);
    }

    paal::lp::glp lp;
    paal::lp::col_id X, Y;
    paal::lp::row_id row;
    paal::lp::linear_expression expr, expr1, expr2;
};
}

#endif /* COMPILATION_ERROR_SETUP_HPP */

