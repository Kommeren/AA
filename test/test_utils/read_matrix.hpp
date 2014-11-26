//=======================================================================
// Copyright (c) 2015
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file read_matrix.hpp
 * @brief
 * @author Tomasz Strozak
 * @version 1.0
 * @date 2015-05-20
 */
#ifndef PAAL_READ_MATRIX_HPP
#define PAAL_READ_MATRIX_HPP

#include "paal/utils/irange.hpp"

#include <boost/numeric/ublas/matrix.hpp>

#include <cassert>
#include <istream>

namespace paal {

template <typename Matrix = boost::numeric::ublas::matrix<double>>
auto read_matrix(std::istream &input_stream) {
    assert(input_stream.good());
    int n, m;
    input_stream >> n >> m;
    Matrix matrix(n, m);
    for (auto i : paal::irange(n)) {
        for (auto j : paal::irange(m)) {
            input_stream >> matrix(i, j);
        }
    }
    return matrix;
}

} //!paal

#endif // PAAL_READ_MATRIX_HPP
