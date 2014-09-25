//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file linear_programming_test.cpp
 * @brief
 * @author Piotr Godlewski
 * @version 1.0
 * @date 2014-04-07
 */

#include "utils/logger.hpp"

#include "paal/lp/glp.hpp"
#include "paal/utils/floating.hpp"

#include <boost/mpl/list.hpp>
#include <boost/range/irange.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/test/unit_test.hpp>

#include <unordered_map>
#include <vector>

using namespace paal;

using lp_types = boost::mpl::list<lp::glp>;

template <typename LP>
void log_solution(lp::problem_type status, const LP &lp_instance) {
    if (status == lp::OPTIMAL) {
        LOGLN("Optimal solution cost: " << lp_instance.get_obj_value());
        for (auto column : lp_instance.get_columns()) {
            boost::ignore_unused_variable_warning(column);
            LOGLN(lp_instance.get_col_name(column)
                  << " = " << lp_instance.get_col_value(column));
        }
    } else {
        LOGLN("Optimal solution not found");
    }
}

BOOST_AUTO_TEST_SUITE(linear_programming_test)

BOOST_AUTO_TEST_CASE_TEMPLATE(linear_programming_example, LP, lp_types) {
    // sample problem
    LP lp_instance;

    lp_instance.set_optimization_type(lp::MAXIMIZE);
    auto X = lp_instance.add_column(500, 0, lp::lp_traits::PLUS_INF, "x");
    auto Y = lp_instance.add_column(300, 0, lp::lp_traits::PLUS_INF, "y");

    auto expr = X + Y;
    lp_instance.add_row(expr >= 7);
    auto row = lp_instance.add_row(expr <= 10);
    lp_instance.add_row(15 <= 200 * X + 100 * Y <= 1200);

    // solve it
    auto status = lp_instance.solve_simplex();
    log_solution(status, lp_instance);

    // add new row
    expr += Y;
    lp_instance.add_row(expr == 12);

    // resolve it
    status = lp_instance.resolve_simplex(lp::DUAL);
    log_solution(status, lp_instance);

    // delete row
    lp_instance.delete_row(row);

    // resolve it
    status = lp_instance.resolve_simplex();
    log_solution(status, lp_instance);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(linear_programming_expressions, LP, lp_types) {
    LP lp_instance;

    lp_instance.set_optimization_type(lp::MAXIMIZE);
    auto X = lp_instance.add_column(500);
    auto Y = lp_instance.add_column(300);
    auto Z = lp_instance.add_column(150);
    auto T = lp_instance.add_column(200);

    auto expr1 = X + Y + 2 * T;
    expr1 -= Y;
    auto expr2 = 2 * expr1 - 3 * Z;
    expr1 += Y * 0.5;
    expr2 += T;
    auto expr3 = expr2 * 0.1 - (Y * 0.3 + 2 * X) * 0.5 + 1 * expr1;

    auto row1 = lp_instance.add_row(expr1 >= 7);
    auto row2 = lp_instance.add_row(expr1 <= 10);
    auto row3 = lp_instance.add_row(expr2 == 8.5);
    auto row4 = lp_instance.add_row(-10 <= expr3 <= 20);

    expr1 = lp::linear_expression();
    expr1 += X;
    expr1 -= T * 1;
    expr1 = expr1 + Z;
    expr1 = T + expr1;

    auto row5 = lp_instance.add_row(expr1 <= 100);

    auto U = lp_instance.add_column(-50);

    auto expr4 =
        -U + U + 0.2 * (X * 7 + (U + X) * 3) + U + 2 * U + 5 * X + (T - T) * 11;

    auto row6 = lp_instance.add_row(expr4 >= 3);

    lp_instance.set_row_expression(
        row2, lp_instance.get_row_expression(row2) + 5 * U);
    lp_instance.set_row_expression(
        row3, lp_instance.get_row_expression(row3) - 2 * U);

    // solve it
    lp_instance.solve_simplex();

    std::vector<lp::col_id> columns = { X, Y, Z, T, U };
    std::vector<lp::row_id> rows = { row1, row2, row3, row4, row5, row6 };
    std::vector<std::vector<double>> coefs = {
        { 1, 0.5, 0, 2, 0 }, { 1, 0.5, 0, 2, 5 }, { 2, 0, -3, 5, -2 },
        { 0.2, 0.35, -0.3, 2.5, 0 }, { 1, 0, 1, 0, 0 }, { 7, 0, 0, 0, 3.6 },
    };
    std::vector<int> row_degrees = { 3, 4, 4, 4, 2, 2 };
    std::vector<int> col_degrees = { 6, 3, 3, 4, 3 };
    const double MAX = lp::lp_traits::PLUS_INF;
    const double MIN = lp::lp_traits::MINUS_INF;
    std::vector<double> lower_bounds = { 7, MIN, 8.5, -10, MIN, 3 };
    std::vector<double> upper_bounds = { MAX, 10, 8.5, 20, 100, MAX };

    for (int row_num : boost::irange(0, int(rows.size()))) {
        auto expr = lp_instance.get_row_expression(rows[row_num]);
        for (int col_num : boost::irange(0, int(columns.size()))) {
            BOOST_CHECK_SMALL(expr.get_coefficient(columns[col_num]) -
                                  coefs[row_num][col_num],
                              std::numeric_limits<double>::epsilon());
        }
        if (lower_bounds[row_num] == MIN) {
            BOOST_CHECK(lp_instance.get_row_lower_bound(rows[row_num]) ==
                        lower_bounds[row_num]);
        } else {
            BOOST_CHECK_SMALL(lp_instance.get_row_lower_bound(rows[row_num]) -
                                  lower_bounds[row_num],
                              std::numeric_limits<double>::epsilon());
        }
        if (upper_bounds[row_num] == MAX) {
            BOOST_CHECK(lp_instance.get_row_upper_bound(rows[row_num]) ==
                        upper_bounds[row_num]);
        } else {
            BOOST_CHECK_SMALL(lp_instance.get_row_upper_bound(rows[row_num]) -
                                  upper_bounds[row_num],
                              std::numeric_limits<double>::epsilon());
        }
        BOOST_CHECK_EQUAL(lp_instance.get_row_degree(rows[row_num]),
                          row_degrees[row_num]);
    }

    for (int col_num : boost::irange(0, int(columns.size()))) {
        BOOST_CHECK_EQUAL(lp_instance.get_col_degree(columns[col_num]),
                          col_degrees[col_num]);
    }
}

template <typename LP>
void run_single_solve_test(lp::simplex_type solve_type,
                           lp::simplex_type resolve_type) {
    LP lp("test instance", lp::MAXIMIZE);
    auto X = lp.add_column(500, 0, lp::lp_traits::PLUS_INF, "x");
    auto Y = lp.add_column(300, 0, lp::lp_traits::PLUS_INF, "y");
    lp.add_row(10 >= X + Y >= 7);
    lp.add_row(200 * X + 100 * Y <= 1200);

    auto status = lp.solve_simplex(solve_type);
    BOOST_CHECK_EQUAL(status, lp::OPTIMAL);
    BOOST_CHECK_SMALL(lp.get_obj_value() - 3400,
                      std::numeric_limits<double>::epsilon());
    BOOST_CHECK_SMALL(lp.get_col_value(X) - 2,
                      4 * std::numeric_limits<double>::epsilon());
    BOOST_CHECK_SMALL(lp.get_col_value(Y) - 8,
                      10 * std::numeric_limits<double>::epsilon());

    lp.add_row(12 >= X + 2 * Y);

    status = lp.resolve_simplex(resolve_type);
    BOOST_CHECK_EQUAL(status, lp::OPTIMAL);
    BOOST_CHECK_SMALL(lp.get_obj_value() - 3200,
                      std::numeric_limits<double>::epsilon());
    BOOST_CHECK_SMALL(lp.get_col_value(X) - 4,
                      std::numeric_limits<double>::epsilon());
    BOOST_CHECK_SMALL(lp.get_col_value(Y) - 4,
                      std::numeric_limits<double>::epsilon());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(linear_programming_solve, LP, lp_types) {
    run_single_solve_test<LP>(lp::PRIMAL, lp::PRIMAL);
    run_single_solve_test<LP>(lp::PRIMAL, lp::DUAL);
    run_single_solve_test<LP>(lp::DUAL, lp::PRIMAL);
    run_single_solve_test<LP>(lp::DUAL, lp::DUAL);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(linear_programming_methods, LP, lp_types) {
    LP lp("test instance", lp::MAXIMIZE);
    // add rows and colums
    auto X = lp.add_column(12, -9, 2.5, "x");
    auto Y = lp.add_column(33, 5, lp::lp_traits::PLUS_INF, "y");
    auto row1 = lp.add_row(X + 3.5 * Y, "row1");
    auto row2 = lp.add_row(0 >= (Y + 2 * X) * 0.1, "row2");
    auto row3 = lp.add_row(1 <= 0.04 * X + 23.7 * Y, "row3");
    auto row4 = lp.add_row(-1 <= Y + 0.2 * X <= 11.2, "row4");

    BOOST_CHECK_EQUAL(lp.columns_number(), 2);
    std::unordered_map<lp::col_id, std::string> columns(
        { { X, "x" }, { Y, "y" } });
    for (auto col : lp.get_columns()) {
        BOOST_CHECK(columns.find(col) != columns.end());
        BOOST_CHECK_EQUAL(lp.get_col_name(col), columns.find(col)->second);
        BOOST_CHECK_EQUAL(lp.get_col_degree(col), 4);
        BOOST_CHECK_EQUAL(boost::distance(lp.get_rows_in_column(col)), 4);
    }

    BOOST_CHECK_EQUAL(lp.rows_number(), 4);
    std::unordered_map<lp::row_id, std::string> rows(
        { { row1, "row1" }, { row2, "row2" }, { row3, "row3" },
          { row4, "row4" } });
    for (auto row : lp.get_rows()) {
        BOOST_CHECK(rows.find(row) != rows.end());
        BOOST_CHECK_EQUAL(lp.get_row_name(row), rows.find(row)->second);
        BOOST_CHECK_EQUAL(lp.get_row_degree(row), 2);
        BOOST_CHECK_EQUAL(lp.get_row_expression(row).non_zeros(), 2);
    }

    // delete some rows and colums
    lp.delete_row(row2);
    BOOST_CHECK_EQUAL(lp.rows_number(), 3);

    lp.delete_col(X);
    BOOST_CHECK_EQUAL(lp.columns_number(), 1);

    lp.delete_row(row3);
    BOOST_CHECK_EQUAL(lp.rows_number(), 2);

    columns = std::unordered_map<lp::col_id, std::string>({ { Y, "y" } });
    for (auto col : lp.get_columns()) {
        BOOST_CHECK(columns.find(col) != columns.end());
        BOOST_CHECK_EQUAL(lp.get_col_name(col), columns.find(col)->second);
        BOOST_CHECK_EQUAL(lp.get_col_degree(col), 2);
        BOOST_CHECK_EQUAL(boost::distance(lp.get_rows_in_column(col)), 2);
    }

    rows = std::unordered_map<lp::row_id, std::string>(
        { { row1, "row1" }, { row4, "row4" } });
    for (auto row : lp.get_rows()) {
        BOOST_CHECK(rows.find(row) != rows.end());
        BOOST_CHECK_EQUAL(lp.get_row_name(row), rows.find(row)->second);
        BOOST_CHECK_EQUAL(lp.get_row_degree(row), 1);
        BOOST_CHECK_EQUAL(lp.get_row_expression(row).non_zeros(), 1);
    }

    // set new bounds and costs
    lp.set_row_lower_bound(row1, 7);
    lp.set_row_upper_bound(row1, 10);
    lp.set_row_lower_bound(row4, lp::lp_traits::MINUS_INF);
    lp.set_row_upper_bound(row4, 12);

    lp.set_col_lower_bound(Y, 0);
    lp.set_col_upper_bound(Y, lp::lp_traits::PLUS_INF);
    lp.set_col_cost(Y, 30);

    // add new column
    auto Z = lp.add_column(50, 0, lp::lp_traits::PLUS_INF, "z");

    // set row linear expressions
    lp.set_row_expression(row1, Y + Z);
    lp.set_row_expression(row4, 2 * Z + Y);

    // solve
    auto status = lp.solve_simplex(lp::PRIMAL);
    BOOST_CHECK_EQUAL(status, lp::OPTIMAL);
    BOOST_CHECK_SMALL(lp.get_obj_value() - 340,
                      std::numeric_limits<double>::epsilon());
    BOOST_CHECK_SMALL(lp.get_row_sum(row1) - 10,
                      std::numeric_limits<double>::epsilon());
    BOOST_CHECK_SMALL(lp.get_row_sum(row4) - 12,
                      std::numeric_limits<double>::epsilon());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(linear_programming_zeros, LP, lp_types) {
    LP lp("test instance", lp::MAXIMIZE);
    auto X = lp.add_column();
    auto Y = lp.add_column();
    auto Z = lp.add_column();
    auto expr = 1e-8 * X + Y;
    BOOST_CHECK_EQUAL(expr.non_zeros(), 2);
    expr += 0 * Z;
    BOOST_CHECK_EQUAL(expr.non_zeros(), 2);
    expr *= 1e-8;
    BOOST_CHECK_EQUAL(expr.non_zeros(), 1);

    expr = (1e-8 * X + Y) / 1e8;
    BOOST_CHECK_EQUAL(expr.non_zeros(), 1);

    expr = X + Y - Y;
    BOOST_CHECK_EQUAL(expr.non_zeros(), 1);
}

BOOST_AUTO_TEST_SUITE_END()
