/**
 * @file read_lp.hpp
 * @brief
 * @author Piotr Godlewski
 * @version 1.0
 * @date 2014-04-03
 */
#ifndef READ_LP_HPP
#define READ_LP_HPP

#include "paal/lp/constraints.hpp"

#include <string>

namespace paal {

template <typename RowBounds>
inline std::string read_rows(std::istream &is, RowBounds &row_bounds,
                             std::string &cost_name) {

    std::string row_type, row_name;
    std::pair<double, double> bounds = { 0, 0 };
    while (true) {
        is >> row_type;
        if (row_type == "L") {
            bounds = { lp::lp_traits::MINUS_INF, 0. };
        } else if (row_type == "G") {
            bounds = { 0., lp::lp_traits::PLUS_INF };
        } else if (row_type == "E") {
            bounds = { 0., 0. };
        } else if (row_type != "N") { // new section name
            return row_type;
        }

        is >> row_name;
        if (row_type == "N") { // row representing the cost function
            cost_name = row_name;
        } else {
            row_bounds[row_name] = bounds;
        }
    }
}

template <typename RowBounds, typename ColBounds, typename CostCoefs,
          typename Coefficients>
inline std::string read_columns(std::istream &is, const RowBounds &row_bounds,
                                std::string cost_name, CostCoefs &cost_coefs,
                                ColBounds &col_bounds,
                                Coefficients &coefficients) {

    std::string col_name, row_name;
    auto save_coef = [&](double new_coef) {
        if (row_name == cost_name) { // row representing the cost function
            cost_coefs[col_name] = new_coef;
        } else {
            col_bounds[col_name] = { 0., lp::lp_traits::PLUS_INF };
            coefficients.insert(
                std::make_pair(row_name, std::make_pair(col_name, new_coef)));
        }
    };

    double coef;
    is >> col_name;
    while (true) {
        if (col_name == "RHS") { // new section name
            return col_name;
        }

        is >> row_name >> coef;
        save_coef(coef);

        is >> row_name;
        if (row_bounds.find(row_name) != row_bounds.end() ||
            row_name == cost_name) {
            is >> coef;
            save_coef(coef);
            is >> col_name;
        } else {
            col_name = row_name;
        }
    }
}

template <typename RowBounds>
inline std::string read_row_bounds(std::istream &is, RowBounds &row_bounds) {

    std::string line_name, row_name;
    auto save_bound = [&](double new_bound) {
        if (row_bounds[row_name].first == 0.) {
            row_bounds[row_name].first = new_bound;
        }
        if (row_bounds[row_name].second == 0.) {
            row_bounds[row_name].second = new_bound;
        }
    };

    double bound;
    is >> line_name;
    while (true) {
        if (line_name == "BOUNDS" ||
            line_name == "ENDATA") { // new section name
            return line_name;
        }

        is >> row_name >> bound;
        save_bound(bound);

        is >> row_name;
        if (row_bounds.find(row_name) != row_bounds.end()) {
            is >> bound;
            save_bound(bound);
            is >> line_name;
        } else {
            line_name = row_name;
        }
    }
}

template <typename ColBounds>
inline std::string read_col_bounds(std::istream &is, ColBounds &col_bounds) {
    std::string col_type, col_name;
    std::pair<double, double> bounds = { 0., 0. };
    bool free_column;
    double bound;

    while (true) {
        free_column = false;
        is >> col_type;

        if (col_type == "LO") {
            bounds = { 0., lp::lp_traits::PLUS_INF };
        } else if (col_type == "UP") {
            bounds = { lp::lp_traits::MINUS_INF, 0. };
        } else if (col_type == "FX") {
            bounds = { 0., 0. };
        } else if (col_type == "FR") {
            bounds = { lp::lp_traits::MINUS_INF, lp::lp_traits::PLUS_INF };
            free_column = true;
        } else { // new section name
            return col_type;
        }

        is >> col_name >> col_name;
        if (!free_column) {
            is >> bound;
            if (bounds.first == 0.) {
                bounds.first = bound;
            } else if (bounds.first == lp::lp_traits::MINUS_INF) {
                bounds.first = 0;
            }
            if (bounds.second == 0.) {
                bounds.second = bound;
            }
        }
        col_bounds[col_name] = bounds;
    }
}

template <typename RowBounds, typename ColBounds, typename CostCoefs,
          typename Coefficients>
inline void read_lp(std::istream &is, RowBounds &row_bounds,
                    ColBounds &col_bounds, CostCoefs &cost_coefs,
                    Coefficients &coefficients) {

    int MAX_LINE = 256;
    char buf[MAX_LINE];
    is.getline(buf, MAX_LINE);

    std::string section_name, cost_name;

    is >> section_name;
    assert(section_name == "ROWS");
    section_name = read_rows(is, row_bounds, cost_name);
    assert(section_name == "COLUMNS");
    section_name = read_columns(is, row_bounds, cost_name, cost_coefs,
                                col_bounds, coefficients);
    assert(section_name == "RHS");
    section_name = read_row_bounds(is, row_bounds);
    assert(section_name == "BOUNDS" || section_name == "ENDATA");
    if (section_name == "BOUNDS") {
        section_name = read_col_bounds(is, col_bounds);
        assert(section_name == "ENDATA");
    }
}
}
#endif /* READ_LP_HPP */
