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
#include "test_utils/read_matrix.hpp"
#include "test_utils/sketch_accuracy_check.hpp"
#include "test_utils/system.hpp"

#include "paal/sketch/frequent_directions.hpp"
#include "paal/utils/irange.hpp"
#include "paal/utils/parse_file.hpp"

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/test/unit_test.hpp>

#include <cassert>
#include <istream>
#include <random>
#include <string>

using coordinate_t = double;
using distribution_t = std::uniform_int_distribution<std::size_t>;

namespace {
    std::default_random_engine generator;
    coordinate_t EPS = 1e-8;
}

auto generate_random_matrix(std::size_t rows_count, std::size_t columns_count) {
    boost::numeric::ublas::matrix<coordinate_t> matrix(rows_count, columns_count);

    std::uniform_real_distribution<coordinate_t> distribution(0,1);

    for (auto i : paal::irange(rows_count)) {
        for (auto j : paal::irange(columns_count)) {
            matrix(i,j) = distribution(generator);
        }
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

        auto data = paal::read_matrix(ifs);

        int sketch_size;
        is_test_cases >> sketch_size;
        auto fd_sketch = paal::make_frequent_directions<coordinate_t>(sketch_size, data.size2());
        fd_sketch.update(data);
        fd_sketch.compress();

        auto sketch = fd_sketch.get_sketch().first;
        check_frequent_directions(std::move(data), std::move(sketch), EPS);
    });
}
