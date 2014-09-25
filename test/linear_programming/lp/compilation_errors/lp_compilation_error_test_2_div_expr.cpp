//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
#include "linear_programming/lp/lp_compilation_error_setup.hpp"

BOOST_FIXTURE_TEST_SUITE(lp_compilation_error, compilation_error_setup)

BOOST_AUTO_TEST_CASE(lp_compilation_error_2_div_expr) {
#ifdef _COMPILATION_ERROR_TEST_
    expr = 2 / expr1;
#else
    expr = 2 * expr1;
#endif
}

BOOST_AUTO_TEST_SUITE_END()
