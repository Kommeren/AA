//=======================================================================
// Copyright (c) 2015
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file frequent_directions_test.cpp
 * @brief
 * @author Tomasz Strozak
 * @version 1.0
 * @date 2015-04-17
 */

#include "test_utils/sketch_accuracy_check.hpp"
#include "test_utils/serialization.hpp"

#include "paal/sketch/frequent_directions.hpp"
#include "paal/utils/irange.hpp"

#include <boost/algorithm/cxx11/all_of.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/algorithm/equal.hpp>
#include <boost/test/unit_test.hpp>

#include <vector>

using coordinate_t = double;
using matrix_t = boost::numeric::ublas::matrix<coordinate_t>;
using matrix_row_t = boost::numeric::ublas::matrix_row<matrix_t>;
using data_t = std::vector<std::vector<coordinate_t>>;

namespace {
    std::size_t const rows_count = 5;
    std::size_t const columns_count = 3;
    std::size_t const sketch_size = 4;
    auto compress_sizes = {1, 2, 3};
    /* Data is chosen so that the sketch matrix is non-degenerate before compress
     * stage. Right-singular vectors are unique up to sign, so the result matrix
     * rows are unique up to sign. */
    data_t const data = {{24, 12, 24},
                           {-40, 40, 20},
                           {0, 0 ,0},
                           {-37, -74, 74},
                           {10, 20, 30}};
    data_t const result = {{-35, -70, 70},
                             {-32, 32, 16},
                             {10, 20, 30},
                             {0, 0, 0}};
    coordinate_t EPS = 1e-9;
}

auto generate_data_matrix() {
    matrix_t matrix(rows_count, columns_count);
    for (auto i : paal::irange(data.size())) {
        matrix_row_t row(matrix, i);
        boost::copy(data[i], row.begin());
    }
    return matrix;
}

BOOST_AUTO_TEST_SUITE(frequent_directions)

BOOST_AUTO_TEST_CASE(update) {
    matrix_t matrix(sketch_size, columns_count);
    paal::frequent_directions<coordinate_t> fd_sketch(std::move(matrix));
    auto data_matrix = generate_data_matrix();

    fd_sketch.update(data_matrix);
    fd_sketch.compress();

    auto sketch = fd_sketch.get_sketch().first;
    check_frequent_directions(std::move(data_matrix), std::move(sketch), EPS);
}

BOOST_AUTO_TEST_CASE(update_row) {
    matrix_t matrix(sketch_size, columns_count);
    paal::frequent_directions<coordinate_t> fd_sketch(std::move(matrix));
    auto data_matrix = generate_data_matrix();

    for (auto i : paal::irange(data_matrix.size1()))
    {
        matrix_row_t row(data_matrix, i);
        fd_sketch.update_row(row);
    }
    fd_sketch.compress();

    auto sketch = fd_sketch.get_sketch().first;
    check_frequent_directions(std::move(data_matrix), std::move(sketch), EPS);
}

BOOST_AUTO_TEST_CASE(zero_rows) {
    matrix_t matrix(sketch_size, columns_count);
    paal::frequent_directions<coordinate_t> fd_sketch(std::move(matrix));
    auto data_matrix = generate_data_matrix();

    fd_sketch.update(std::move(data_matrix));
    fd_sketch.compress();

    auto actual_size = fd_sketch.get_sketch().second;
    BOOST_CHECK(actual_size <= sketch_size/2);

    auto sketch = fd_sketch.get_sketch().first;
    for (auto i : paal::irange(actual_size, sketch_size)) {
        matrix_row_t row(sketch, i);
        BOOST_CHECK(boost::algorithm::all_of_equal(row, 0.0));
    }
}

BOOST_AUTO_TEST_CASE(result_matrix) {
    matrix_t matrix(sketch_size, columns_count);
    paal::frequent_directions<coordinate_t> fd_sketch(std::move(matrix));
    auto data_matrix = generate_data_matrix();

    fd_sketch.update(std::move(data_matrix));

    auto sketch = fd_sketch.get_sketch().first;

    auto check_close_doubles = [](double l, double r) {
        return std::abs(l - r) < EPS;
    };

    for (auto i : paal::irange(sketch_size)) {
        matrix_row_t row (sketch, i);
        BOOST_CHECK(boost::equal(result[i], row, check_close_doubles) ||
                    boost::equal(result[i], -row, check_close_doubles));
    };
}

BOOST_AUTO_TEST_CASE(sparse) {
    matrix_t matrix(sketch_size, columns_count, 0);
    paal::frequent_directions<coordinate_t> fd_sketch(std::move(matrix));
    boost::numeric::ublas::mapped_matrix<coordinate_t> data_matrix(rows_count, columns_count);
    for (auto i : paal::irange(rows_count)) {
        data_matrix(i, i % columns_count) = i + 1;
    }

    fd_sketch.update(data_matrix);
    fd_sketch.compress();

    auto sketch = fd_sketch.get_sketch().first;
    check_frequent_directions(std::move(data_matrix), std::move(sketch), EPS);
}

BOOST_AUTO_TEST_CASE(zero_matrix) {
    matrix_t matrix(sketch_size, columns_count);
    paal::frequent_directions<coordinate_t> fd_sketch(std::move(matrix));
    matrix_t data_matrix(rows_count, columns_count, 0);

    fd_sketch.update(data_matrix);
    fd_sketch.compress();

    auto sketch = fd_sketch.get_sketch().first;
    check_frequent_directions(std::move(data_matrix), std::move(sketch), EPS);
}

BOOST_AUTO_TEST_CASE(compress_size) {
    for(auto compress_size : compress_sizes) {
        matrix_t matrix(sketch_size, columns_count);
        paal::frequent_directions<coordinate_t> fd_sketch(std::move(matrix), compress_size);
        auto data_matrix = generate_data_matrix();

        fd_sketch.update(data_matrix);
        fd_sketch.compress();

        auto sketch = fd_sketch.get_sketch().first;
        check_frequent_directions(std::move(data_matrix), std::move(sketch),
                EPS, coordinate_t(compress_size));
    }
}

BOOST_AUTO_TEST_CASE(serialization) {
    matrix_t matrix(sketch_size, columns_count);
    paal::frequent_directions<coordinate_t> fd_sketch(std::move(matrix));
    auto data_matrix = generate_data_matrix();

    fd_sketch.update(std::move(data_matrix));
    fd_sketch.compress();

    serialize_test(fd_sketch, "tmp.bin");
}

BOOST_AUTO_TEST_SUITE_END()
