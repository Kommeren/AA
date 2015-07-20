//=======================================================================
// Copyright (c) 2015
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================

/**
 * @file print_collection_test.cpp
 * @brief Testing print_collection
 * @author Tomasz Strozak
 * @version 1.0
 * @date 2015-08-16
 */

#include "paal/utils/irange.hpp"
#include "paal/utils/print_collection.hpp"

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/test/unit_test.hpp>

#include <cassert>
#include <sstream>
#include <string>

namespace {

using coordinate_t = double;
using vector_t = std::vector<coordinate_t>;
using matrix_t = boost::numeric::ublas::matrix<coordinate_t>;

void test_print_collection(vector_t const &collection,
                           std::string const &expected_result,
                           std::string const &delimiter) {
    std::stringstream stream;
    paal::print_collection(stream, collection, delimiter);
    BOOST_CHECK(stream.good());
    BOOST_CHECK_EQUAL(stream.str(), expected_result);
}

auto to_ublas_matrix(std::vector<vector_t> const &data) {
    assert(!data.empty());
    matrix_t matrix(data.size(), data.front().size());

    for (auto i : paal::irange(data.size())) {
        boost::numeric::ublas::matrix_row<matrix_t> row(matrix, i);
        boost::copy(data[i], row.begin());
    }
    return matrix;
}

void test_print_matrix(matrix_t const &matrix,
                             std::string const &expected_result,
                             std::string const &delimiter) {
    std::stringstream stream;
    paal::print_matrix(stream, matrix, delimiter);
    BOOST_CHECK(stream.good());
    BOOST_CHECK_EQUAL(stream.str(), expected_result);
}

} // anonymous namespace

BOOST_AUTO_TEST_SUITE(print_collection_tests)

BOOST_AUTO_TEST_CASE(collection_space) {
    ::test_print_collection({1,2,3}, "1 2 3", " ");
}

BOOST_AUTO_TEST_CASE(collection_comma) {
    ::test_print_collection({1,2,3}, "1,2,3", ",");
}

BOOST_AUTO_TEST_CASE(collection_empty) {
    ::test_print_collection({}, "", " ");
}

BOOST_AUTO_TEST_CASE(matrix_space) {
    auto matrix = to_ublas_matrix({{1, 2}, {3, 4}});
    ::test_print_matrix(matrix, "1 2\n3 4", " ");
}

BOOST_AUTO_TEST_CASE(matrix_comma) {
    auto matrix = to_ublas_matrix({{1, 2}, {3, 4}});
    ::test_print_matrix(matrix, "1,2\n3,4", ",");
}

BOOST_AUTO_TEST_CASE(matrix_empty) {
    matrix_t matrix {0,0};
    ::test_print_matrix(matrix, "", " ");
}

BOOST_AUTO_TEST_SUITE_END()

