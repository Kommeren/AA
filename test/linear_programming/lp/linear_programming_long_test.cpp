//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file linear_programming_long_test.cpp
 * @brief
 * @author Piotr Godlewski
 * @version 1.0
 * @date 2014-04-03
 */

#include "test_utils/logger.hpp"
#include "test_utils/read_lp.hpp"

#include "paal/lp/glp.hpp"
#include "paal/utils/parse_file.hpp"

#include <boost/mpl/list.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/range/numeric.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>

#include <fstream>
#include <unordered_map>

using lp_types = boost::mpl::list<paal::lp::glp>;

template <typename LP, typename RowBounds, typename ColBounds,
          typename CostCoefs, typename Coefficients>
void run_test(RowBounds &row_bounds, ColBounds &col_bounds,
              CostCoefs &cost_coefs, Coefficients &coefs, double best_cost,
              int rows_num, int cols_num, int non_zeros,
              paal::lp::simplex_type type) {

    LP lp("LP long test", paal::lp::MINIMIZE);
    std::unordered_map<std::string, paal::lp::col_id> col_ids;

    std::string col_name;
    for (auto c : col_bounds) {
        col_name = c.first;
        col_ids[col_name] =
            lp.add_column(0., c.second.first, c.second.second, col_name);
    }

    int non_zero_cost_coefs = 0;
    for (auto c : cost_coefs) {
        lp.set_col_cost(col_ids[c.first], c.second);
        if (!paal::utils::compare<double>().e(c.second, 0.)) {
            ++non_zero_cost_coefs;
        }
    }

    std::string row_name;
    std::pair<double, double> row_bound;
    for (auto row : row_bounds) {
        std::tie(row_name, row_bound) = row;

        paal::lp::linear_expression expr;

        for (auto elem :
             boost::make_iterator_range(coefs.equal_range(row_name))) {
            expr += elem.second.second * col_ids[elem.second.first];
        }

        lp.add_row(row_bound.first <= std::move(expr) <= row_bound.second);
    }

    BOOST_CHECK_EQUAL(rows_num, lp.rows_number() + 1);
    BOOST_CHECK_EQUAL(cols_num, lp.columns_number());
    auto lp_non_zeros =
        boost::accumulate(lp.get_rows(), 0, [&](int sum, paal::lp::row_id row) {
        return sum + lp.get_row_degree(row);
    });
    BOOST_CHECK_EQUAL(non_zeros, lp_non_zeros + non_zero_cost_coefs);

    auto status = lp.solve_simplex(type);
    BOOST_CHECK_EQUAL(status, paal::lp::OPTIMAL);
    static const double EPSILON = 6e-5;
    BOOST_CHECK_CLOSE(best_cost, lp.get_obj_value(), EPSILON);
}

BOOST_AUTO_TEST_SUITE(linear_programming_long)

BOOST_AUTO_TEST_CASE_TEMPLATE(linear_programming_long, LP, lp_types) {
    std::string testDir = "test/data/LP/";
    paal::parse(testDir + "cases.txt",
                [&](const std::string & fname, std::istream & is_test_cases) {
        double best_cost;
        int rows_num, cols_num, non_zeros;
        is_test_cases >> best_cost >> rows_num >> cols_num >> non_zeros;

        LOGLN(fname);
        std::ifstream ifs(testDir + "/cases/" + fname + ".mps");

        std::unordered_map<std::string, std::pair<double, double>> row_bounds;
        std::unordered_map<std::string, std::pair<double, double>> col_bounds;
        std::unordered_map<std::string, double> cost_coefs;
        std::unordered_multimap<std::string, std::pair<std::string, double>>
            coefs;

        paal::read_lp(ifs, row_bounds, col_bounds, cost_coefs, coefs);
        LOGLN("primal");
        run_test<LP>(row_bounds, col_bounds, cost_coefs, coefs, best_cost,
                     rows_num, cols_num, non_zeros, paal::lp::PRIMAL);
        LOGLN("dual");
        run_test<LP>(row_bounds, col_bounds, cost_coefs, coefs, best_cost,
                     rows_num, cols_num, non_zeros, paal::lp::DUAL);
    });
}

BOOST_AUTO_TEST_SUITE_END()
