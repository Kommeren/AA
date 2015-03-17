//=======================================================================
// Copyright (c) 2015 Piotr Wygocki
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================

#include "paal/lp/glp.hpp"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(lp_compilation_error)

//This macro helps checking that particular expression does not compile.
//Exactly one from EXPR1 and EXPR2 should compile
//one exactly one shouldn't.
//
#define CHECK_COMP_ERROR(UNIQUE_HELPER_FUN_NAME, EXPR1, EXPR2) \
template <typename Lp, typename Expr, typename Expr1, typename Expr2, typename XX, typename YY, typename Row> \
auto UNIQUE_HELPER_FUN_NAME(Lp & lp, Expr & expr, Expr1 & expr1, Expr2 & expr2, XX & X, YY & Y, Row & row) -> decltype(EXPR1) { return EXPR1; }\
\
template <typename Lp, typename Expr, typename Expr1, typename Expr2, typename XX, typename YY, typename Row> \
auto UNIQUE_HELPER_FUN_NAME(Lp & lp, Expr & expr, Expr1 & expr1, Expr2 & expr2, XX & X, YY & Y, Row & row) -> decltype(EXPR2) { return EXPR2; }\
\
BOOST_AUTO_TEST_CASE(UNIQUE_HELPER_FUN_NAME##_test) { \
    paal::lp::glp lp;\
    paal::lp::linear_expression expr, expr1, expr2;\
    auto X = lp.add_column();\
    auto Y = lp.add_column();\
    auto row = lp.add_row(X);\
    UNIQUE_HELPER_FUN_NAME(lp, expr, expr1, expr2, X, Y, row); \
}

CHECK_COMP_ERROR(F1, expr / X, expr + X)
CHECK_COMP_ERROR(F2, lp.add_row(1 == row), lp.add_row(1 == X))
CHECK_COMP_ERROR(F3, lp.add_row(1 >= row), lp.add_row(1 >= X))
CHECK_COMP_ERROR(F4, lp.add_row(1 <= row), lp.add_row(1 <= X))
CHECK_COMP_ERROR(F5, expr = 2 / expr1, expr = 2 * expr1)
CHECK_COMP_ERROR(F6, expr = 2 / X, expr = 2 * X)
CHECK_COMP_ERROR(F7, lp.add_row(2 >= (expr <= 1)), lp.add_row(2 >= (expr >= 1)))
CHECK_COMP_ERROR(F8, lp.add_row(2 <= (expr >= 1)), lp.add_row(2 <= (expr <= 1)))
CHECK_COMP_ERROR(F9, expr = 2 - X, expr = 2 * X)
CHECK_COMP_ERROR(F10, expr = 2 * row, expr = 2 * X)
CHECK_COMP_ERROR(F11, expr = 2 + X, expr = 2 * X)
CHECK_COMP_ERROR(F12, lp.add_row(3 >= expr >= 2 * Y), lp.add_row(3 >= expr))
CHECK_COMP_ERROR(F13, lp.add_row(3 >= expr >= 2 * Y), (lp.add_row(3 >= expr), lp.add_row(expr - 2 * Y >= 0)))
CHECK_COMP_ERROR(F14, lp.add_row(6 == expr >= 5), lp.add_row(6 >= expr >= 5))
CHECK_COMP_ERROR(F15, lp.add_row(6 == expr <= 7), lp.add_row(6 <= expr <= 7))
CHECK_COMP_ERROR(F16, lp.add_row(6 >= expr == 5), lp.add_row(6 >= expr >= 5))
CHECK_COMP_ERROR(F17, lp.add_row(6 >= expr <= 5), lp.add_row(6 >= expr >= 5))
CHECK_COMP_ERROR(F18, lp.add_row(6 <= expr == 7), lp.add_row(6 <= expr <= 7))
CHECK_COMP_ERROR(F19, lp.add_row(6 <= expr >= 5), lp.add_row(6 >= expr >= 5))
CHECK_COMP_ERROR(F20, expr = row, expr = X)
CHECK_COMP_ERROR(F21, expr /= expr1, expr -= expr1)
CHECK_COMP_ERROR(F22, expr /= X, expr /= 2)
CHECK_COMP_ERROR(F23, expr = expr1 / expr2, expr = expr1 + expr2)
CHECK_COMP_ERROR(F24, lp.add_row(expr == expr1), lp.add_row(expr - expr1 == 0))
CHECK_COMP_ERROR(F25, lp.add_row(expr >= 5 >= 6), lp.add_row(6 >= expr >= 5))
CHECK_COMP_ERROR(F26, lp.add_row(expr >= 5 <= 6), lp.add_row(6 >= expr >= 5))
CHECK_COMP_ERROR(F27, lp.add_row(expr >= expr1), lp.add_row(expr - expr1 >= 0))
CHECK_COMP_ERROR(F28, lp.add_row(expr <= 5 <= 6), lp.add_row(5 <= expr <= 6))
CHECK_COMP_ERROR(F29, lp.add_row(expr <= 6 >= 5), lp.add_row(5 <= expr <= 6))
CHECK_COMP_ERROR(F30, lp.add_row(expr <= expr1), lp.add_row(expr - expr1 <= 0))
CHECK_COMP_ERROR(F31, expr = expr1 * expr2, expr = expr1 + expr2)
CHECK_COMP_ERROR(F32, expr = expr1 * X, expr = expr1 + X)
CHECK_COMP_ERROR(F33, expr -= 1, expr -= X)
CHECK_COMP_ERROR(F34, expr -= row, expr -= X)
CHECK_COMP_ERROR(F35, expr *= expr1, expr += expr1)
CHECK_COMP_ERROR(F36, expr *= X,  expr *= 2)
CHECK_COMP_ERROR(F37, expr += 1, expr += X)
CHECK_COMP_ERROR(F38, expr += row, expr += X)
CHECK_COMP_ERROR(F39, lp.add_row(row == 1), lp.add_row(X == 1))
CHECK_COMP_ERROR(F40, lp.add_row(row >= 1), lp.add_row(X >= 1))
CHECK_COMP_ERROR(F41, lp.add_row(row <= 1), lp.add_row(X <= 1))
CHECK_COMP_ERROR(F42, expr = +X, expr = -X)
CHECK_COMP_ERROR(F43, expr = X / expr1, expr = X - expr1)
CHECK_COMP_ERROR(F44, expr = X / Y,  expr = X + Y)
CHECK_COMP_ERROR(F45, expr = X - 2, expr = X * 2)
CHECK_COMP_ERROR(F46, expr = X * expr1, expr = X + expr1)
CHECK_COMP_ERROR(F47, expr = X * X, expr = X + X)
CHECK_COMP_ERROR(F48, expr = X * Y, expr = X + Y)
CHECK_COMP_ERROR(F49, expr = X + 2, expr = X * 2)
CHECK_COMP_ERROR(F50, expr = X + row, expr = X + Y)

BOOST_AUTO_TEST_SUITE_END()
