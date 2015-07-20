//=======================================================================
// Copyright (c) 2015
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================

/**
 * @file read_rows_test.cpp
 * @brief Testing read_rows
 * @author Tomasz Strozak
 * @version 1.0
 * @date 2015-07-25
 */
#include "paal/utils/read_rows.hpp"

#include <boost/range/algorithm/equal.hpp>
#include <boost/test/unit_test.hpp>

#include <ios>
#include <string>
#include <utility>
#include <vector>

#include <iostream>

namespace {

using coordinate_t = double;
using row_t = std::vector<coordinate_t>;
using vector_row_t = std::vector<row_t>;

void test_read_rows(
        std::string const &input,
        std::size_t max_rows_to_read,
        vector_row_t const &expected_result,
        bool bad_row,
        std::size_t row_size = 0,
        bool size_from_first_row = true) {
    std::stringstream input_stream(input);

    vector_row_t rows;

    bool contains_bad_row = false;
    auto ignore_bad_row = [&](std::string const &bad_line) {
        contains_bad_row = true;
        return true;
    };

    if(size_from_first_row) {
        paal::read_rows_first_row_size<coordinate_t>(input_stream, rows, max_rows_to_read, ignore_bad_row);
    }
    else {
        paal::read_rows<coordinate_t>(input_stream, rows, row_size, max_rows_to_read, ignore_bad_row);
    }

    BOOST_CHECK_EQUAL(contains_bad_row, bad_row);

    BOOST_CHECK(boost::equal(rows, expected_result));
}

void test_read_rows_and_first_row_size(
        std::string const &input,
        std::size_t max_rows_to_read,
        vector_row_t const &expected_result,
        std::size_t row_size,
        bool bad_row = false) {
    test_read_rows(input, max_rows_to_read, expected_result, bad_row);
    test_read_rows(input, max_rows_to_read, expected_result, bad_row, row_size, false);
}

} // anonymous namespace

BOOST_AUTO_TEST_SUITE(read_rows_tests)

BOOST_AUTO_TEST_CASE(without_newline) {
    ::test_read_rows_and_first_row_size("0 1 2", 5, {{0, 1, 2}}, 3);
}

BOOST_AUTO_TEST_CASE(with_newline) {
    ::test_read_rows_and_first_row_size("0 1 2\n", 5, {{0, 1, 2}}, 3);
}

BOOST_AUTO_TEST_CASE(multiline) {
    ::test_read_rows_and_first_row_size("0 1 2\n2 3 4\n", 5, {{0, 1, 2}, {2, 3, 4}}, 3);
}

BOOST_AUTO_TEST_CASE(spaces_on_end) {
    ::test_read_rows_and_first_row_size("0 1 2 \n2 3 4 ", 5, {{0, 1, 2}, {2, 3, 4}}, 3);
}

BOOST_AUTO_TEST_CASE(ignore_long_line) {
    ::test_read_rows_and_first_row_size("0 1 2\n2 3 4 5", 5, {{0, 1, 2}}, 3, true);
}

BOOST_AUTO_TEST_CASE(ignore_short_line) {
    ::test_read_rows_and_first_row_size("0 1 2\n2 3", 5, {{0, 1, 2}}, 3, true);
}

BOOST_AUTO_TEST_CASE(bad_character) {
    ::test_read_rows_and_first_row_size("0 1 2\n2 a 3 4\n", 5, {{0, 1, 2}}, 3, true);
}

BOOST_AUTO_TEST_SUITE_END()

