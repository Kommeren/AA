#include "linear_programming/lp/lp_compilation_error_setup.hpp"

BOOST_FIXTURE_TEST_SUITE(lp_compilation_error, compilation_error_setup)

BOOST_AUTO_TEST_CASE(lp_compilation_error_6_le_expr_eq_7) {
#ifdef _COMPILATION_ERROR_TEST_
    lp.add_row(6 <= expr == 7);
#else
    lp.add_row(6 <= expr <= 7);
#endif
}

BOOST_AUTO_TEST_SUITE_END()

