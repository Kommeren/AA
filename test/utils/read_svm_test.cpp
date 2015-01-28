//=======================================================================
// Copyright (c) 2015 Andrzej Pacuk
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================

/**
 * @file read_svm_test.cpp
 * @brief Testing read_svm
 * @author Andrzej Pacuk
 * @version 1.0
 * @date 2015-02-10
 */
#include "paal/utils/read_svm.hpp"

#include <boost/test/unit_test.hpp>

#include <ios>
#include <string>
#include <utility>
#include <vector>

namespace {

using coordinates_type = std::vector<double>;
using svm_row = paal::detail::svm_row<coordinates_type>;

void test_read_svm_with_svm_row(
        svm_row &row,
        std::string const &input,
        coordinates_type const &expected_coordinates,
        int const expected_result,
        std::ios::iostate expected_stream_error_state = std::ios::eofbit,
        bool check_contents = true) {
    std::stringstream input_stream(input);
    input_stream >> row;
    BOOST_CHECK_EQUAL(input_stream.rdstate(), expected_stream_error_state);

    if (check_contents) {
        BOOST_CHECK_EQUAL_COLLECTIONS(expected_coordinates.begin(), expected_coordinates.end(), row.get_coordinates().begin(), row.get_coordinates().end());
        BOOST_CHECK_EQUAL(expected_result, row.get_result());
    }
}

void test_read_svm(
        std::size_t const dimensions,
        std::string const &input,
        coordinates_type const &expected_coordinates,
        int const expected_result,
        std::ios::iostate expected_stream_error_state = std::ios::eofbit,
        bool check_contents = true) {
    svm_row row{dimensions};
    test_read_svm_with_svm_row(row, input, expected_coordinates, expected_result, expected_stream_error_state, check_contents);
}

} // anonymous namespace

BOOST_AUTO_TEST_SUITE(read_svm_tests)


BOOST_AUTO_TEST_CASE(correct_without_newline) {
    ::test_read_svm(2, "0 0:4 1:5", {4, 5}, 0);
}

BOOST_AUTO_TEST_CASE(correct_unordered) {
    ::test_read_svm(2, "1 1:5 0:4\n", {4, 5}, 1);
}

BOOST_AUTO_TEST_CASE(correct_with_zeros) {
    ::test_read_svm(5, "1 1:5\n", {0, 5, 0, 0, 0}, 1);
}

BOOST_AUTO_TEST_CASE(correct_spaces_on_end) {
    ::test_read_svm(2, "1 1:5 0:4 ", {4, 5}, 1);
}

BOOST_AUTO_TEST_CASE(correct_with_tabs) {
    ::test_read_svm(2, "1\t1:5\t\t0:4\t", {4, 5}, 1);
}

BOOST_AUTO_TEST_CASE(correct_nullify_previous_row) {
    svm_row row{3};
    ::test_read_svm_with_svm_row(row, "1 0:4 1:5 2:6\n", {4, 5, 6}, 1);
    ::test_read_svm_with_svm_row(row, "0 1:2 2:7\n", {0, 2, 7}, 0);
}

BOOST_AUTO_TEST_CASE(to_big_feature_id) {
    ::test_read_svm(2, "1 0:4 4:99 1:5\n", {4, 5, 0, 0, 99}, 1, std::ios::failbit | std::ios::eofbit);
}

BOOST_AUTO_TEST_CASE(bad_result) {
    ::test_read_svm(2, "abc 0:4 1:5\n", {}, 1, std::ios::failbit, false);
}

BOOST_AUTO_TEST_CASE(bad_feature_id) {
    ::test_read_svm(2, "1 abc:4 1:5\n", {}, 1, std::ios::failbit, false);
}

BOOST_AUTO_TEST_CASE(bad_no_colon) {
    ::test_read_svm(1, "1 0 4\n", {}, 1, std::ios::failbit, false);
}

BOOST_AUTO_TEST_CASE(bad_feature_value) {
    ::test_read_svm(2, "1 0:abc 1:5\n", {}, 1, std::ios::failbit, false);
}

BOOST_AUTO_TEST_CASE(bad_feature_value_on_end) {
    ::test_read_svm(2, "1 0:4 1:a\n", {}, 1, std::ios::failbit, false);
}

BOOST_AUTO_TEST_CASE(bad_two_lines) {
    ::test_read_svm(2, "1 0:4 1:5\n0 0:3 1:2", {4, 5}, 1, std::ios::failbit);
}

BOOST_AUTO_TEST_SUITE_END()

