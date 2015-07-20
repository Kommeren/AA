//=======================================================================
// Copyright (c) 2015
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file frequent_directions_long_test.cpp
 * @brief
 * @author Tomasz Strozak
 * @version 1.0
 * @date 2015-04-19
 */

#include "test_utils/get_test_dir.hpp"
#include "test_utils/sketch_accuracy_check.hpp"
#include "test_utils/system.hpp"

#include "paal/sketch/frequent_directions.hpp"
#include "paal/utils/irange.hpp"
#include "paal/utils/parse_file.hpp"
#include "paal/utils/read_rows.hpp"

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/test/unit_test.hpp>

#include <cassert>
#include <istream>
#include <random>
#include <string>

using coordinate_t = double;
using distribution_t = std::uniform_int_distribution<std::size_t>;
using matrix_t = boost::numeric::ublas::matrix<coordinate_t>;
using row_t = std::vector<coordinate_t>;
using vector_row_t = std::vector<row_t>;

namespace {
    std::default_random_engine generator;
    std::size_t MAX_ROWS = 2000;
    coordinate_t EPS = 1e-7;
}

auto generate_random_matrix(std::size_t rows_count, std::size_t columns_count) {
    matrix_t matrix(rows_count, columns_count);

    std::uniform_real_distribution<coordinate_t> distribution(0,1);

    for (auto i : paal::irange(rows_count)) {
        for (auto j : paal::irange(columns_count)) {
            matrix(i,j) = distribution(generator);
        }
    }

    return matrix;
}

auto generate_matrix_from_row_buffer(vector_row_t const &row_buffer) {
    matrix_t matrix(boost::size(row_buffer), boost::size(row_buffer.front()));

    auto row = row_buffer.begin();
    auto row_matrix = matrix.begin1();
    for (; row != row_buffer.end() && row_matrix != matrix.end1(); ++row, ++row_matrix) {
        std::copy(row->begin(), row->end(), row_matrix.begin());
    }

    return matrix;
}

BOOST_AUTO_TEST_CASE(frequent_directions_random) {
    int test_count = 10;
    std::size_t const min_rows = 10;
    std::size_t const max_rows = 5000;
    std::size_t const min_columns = 10;
    std::size_t const max_columns = 500;
    std::size_t const min_sketch_size = 1;
    distribution_t rows_distribution(min_rows, max_rows);
    distribution_t columns_distribution(min_columns, max_columns);

    while (test_count--) {
        auto rows_count = rows_distribution(generator);
        auto columns_count = columns_distribution(generator);
        distribution_t sketch_size_distribution(min_sketch_size, rows_count/10);
        auto sketch_size = sketch_size_distribution(generator);

        auto fd_sketch = paal::make_frequent_directions<coordinate_t>(sketch_size, columns_count);
        auto data = generate_random_matrix(rows_count, columns_count);
        fd_sketch.update(std::move(data));
        fd_sketch.compress();

        auto sketch = fd_sketch.get_sketch().first;
        check_frequent_directions(std::move(data), std::move(sketch), EPS);
    }
}

BOOST_AUTO_TEST_CASE(frequent_directions_long) {
    std::string test_dir = paal::system::get_test_data_dir("SKETCH");

    using paal::system::build_path;

    paal::parse(build_path(test_dir, "cases.txt"), [&](std::string const & fname,
                std::istream & is_test_cases) {
        std::ifstream ifs(build_path(test_dir, "/cases/" + fname + ".txt"));
        assert(ifs.good());

        std::vector<std::vector<coordinate_t>> row_buffer;
        row_buffer.reserve(MAX_ROWS);
        paal::read_rows_first_row_size<coordinate_t>(ifs, row_buffer, MAX_ROWS);

        auto data = generate_matrix_from_row_buffer(row_buffer);

        std::size_t sketch_size;
        is_test_cases >> sketch_size;
        auto fd_sketch = paal::make_frequent_directions<coordinate_t>(sketch_size, data.size2());
        fd_sketch.update(data);
        fd_sketch.compress();

        auto sketch = fd_sketch.get_sketch().first;
        check_frequent_directions(std::move(data), std::move(sketch), EPS);
    });
}
