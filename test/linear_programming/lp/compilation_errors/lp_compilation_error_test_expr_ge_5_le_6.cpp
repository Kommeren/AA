#include "linear_programming/lp/lp_compilation_error_setup.hpp"

BOOST_FIXTURE_TEST_SUITE(lp_compilation_error, compilation_error_setup)

BOOST_AUTO_TEST_CASE(lp_compilation_error_expr_ge_5_le_6) {
#ifdef _COMPILATION_ERROR_TEST_
    lp.add_row(expr >= 5 <= 6);
#else
    lp.add_row(6 >= expr >= 5);
#endif
}

BOOST_AUTO_TEST_SUITE_END()

