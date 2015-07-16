//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file ublas_traits_test.cpp
 * @brief
 * @author Tomasz Strozak
 * @version 1.0
 * @date 2015-06-21
 */

#include "paal/data_structures/ublas_traits.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_sparse.hpp>

#include <vector>

using coordinate_t = double;
using paal::data_structures::is_sparse_row;
namespace ublas = boost::numeric::ublas;

BOOST_AUTO_TEST_SUITE(sparse_row)

BOOST_AUTO_TEST_CASE(vector) {
    BOOST_CHECK(!is_sparse_row<std::vector<coordinate_t>>());
}

BOOST_AUTO_TEST_CASE(ublas_vector) {
    BOOST_CHECK(!is_sparse_row<ublas::vector<coordinate_t>>());
}

BOOST_AUTO_TEST_CASE(ublas_compressed_vector) {
    BOOST_CHECK(is_sparse_row<ublas::compressed_vector<coordinate_t>>());
}

BOOST_AUTO_TEST_CASE(ublas_matrix_row) {
    BOOST_CHECK(!is_sparse_row<ublas::matrix<coordinate_t>::iterator1>());
}

BOOST_AUTO_TEST_CASE(ublas_compressed_matrix_row) {
    BOOST_CHECK(is_sparse_row<ublas::compressed_matrix<coordinate_t>::iterator1>());
}

BOOST_AUTO_TEST_SUITE_END()
