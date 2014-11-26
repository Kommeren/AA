//=======================================================================
// Copyright (c) 2015
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file sketch_accuracy_check.hpp
 * @brief
 * @author Tomasz Strozak
 * @version 1.0
 * @date 2015-04-21
 */
#ifndef PAAL_SKETCH_ACCURACY_CHECK_HPP
#define PAAL_SKETCH_ACCURACY_CHECK_HPP

#include <boost/algorithm/cxx11/all_of.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_expression.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/bindings/lapack/syevd.hpp>
#include <boost/numeric/bindings/lapack/workspace.hpp>
#include <boost/range/algorithm/max_element.hpp>
#include <boost/test/unit_test.hpp>

template <typename MatrixOriginal,
          typename MatrixSketch>
void check_frequent_directions(MatrixOriginal &&original_matrix,
        MatrixSketch &&sketch, typename MatrixOriginal::value_type epsilon,
        typename MatrixOriginal::value_type ratio) {
    using value_type = typename MatrixOriginal::value_type;

    MatrixOriginal a = boost::numeric::ublas::prod(boost::numeric::ublas::trans(original_matrix), original_matrix);
    MatrixSketch b = boost::numeric::ublas::prod(boost::numeric::ublas::trans(sketch), sketch);
    MatrixSketch diff_matrix = a - b;

    auto n = diff_matrix.size1();
    boost::numeric::ublas::vector<value_type> eigenvalues(n);
    boost::numeric::ublas::matrix<value_type, boost::numeric::ublas::column_major> m{std::move(diff_matrix)};

    //diff_matrix is a symmetric matrix so we can check positive semi-definite
    //and compute second norm using its eigenvalues
    auto info = boost::numeric::bindings::lapack::syevd('N', 'L', m,
        eigenvalues, boost::numeric::bindings::lapack::optimal_workspace());
    BOOST_CHECK(!info);

    //check if diff matrix is positive semi-definite
    BOOST_CHECK(boost::algorithm::all_of(eigenvalues,
            [&](value_type x){ return x > -epsilon; }));

    auto diff_matrix_norm_2 = *boost::max_element(eigenvalues);
    auto original_matrix_norm_frobenius = boost::numeric::ublas::norm_frobenius(original_matrix);

    //check norm inequality
    BOOST_CHECK_LE(diff_matrix_norm_2, original_matrix_norm_frobenius
        * original_matrix_norm_frobenius / ratio);
}

template <typename MatrixOriginal,
          typename MatrixSketch>
void check_frequent_directions(MatrixOriginal &&original_matrix,
        MatrixSketch &&sketch, typename MatrixOriginal::value_type epsilon) {
    check_frequent_directions(std::forward<MatrixOriginal>(original_matrix),
            std::forward<MatrixSketch>(sketch), epsilon,
            (typename MatrixOriginal::value_type)sketch.size1() / 2);
}

#endif // PAAL_SKETCH_ACCURACY_CHECK_HPP
