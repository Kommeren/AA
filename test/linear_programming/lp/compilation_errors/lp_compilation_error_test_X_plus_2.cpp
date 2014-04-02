#include "linear_programming/lp/lp_compilation_error_setup.hpp"

BOOST_FIXTURE_TEST_SUITE(lp_compilation_error, compilation_error_setup)

BOOST_AUTO_TEST_CASE(lp_compilation_error_X_plus_2) {
#ifdef _COMPILATION_ERROR_TEST_
    expr = X + 2;
#else
    expr = X * 2;
#endif
}

BOOST_AUTO_TEST_SUITE_END()

