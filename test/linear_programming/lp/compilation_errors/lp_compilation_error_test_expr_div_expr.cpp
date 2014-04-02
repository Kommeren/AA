#include "linear_programming/lp/lp_compilation_error_setup.hpp"

BOOST_FIXTURE_TEST_SUITE(lp_compilation_error, compilation_error_setup)

BOOST_AUTO_TEST_CASE(lp_compilation_error_expr_div_expr) {
#ifdef _COMPILATION_ERROR_TEST_
    expr = expr1 / expr2;
#else
    expr = expr1 + expr2;
#endif
}

BOOST_AUTO_TEST_SUITE_END()

