//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file frequent_directions.hpp
 * @brief
 * @author Tomasz Strozak
 * @version 1.0
 * @date 2014-11-26
 */
#ifndef PAAL_FREQUENT_DIRECTIONS_HPP
#define PAAL_FREQUENT_DIRECTIONS_HPP

#include "paal/data_structures/ublas_traits.hpp"
#include "paal/utils/irange.hpp"

#include <boost/numeric/bindings/lapack/gesvd.hpp>
#include <boost/numeric/ublas/detail/matrix_assign.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_expression.hpp>

#include <boost/range/algorithm/copy.hpp>
#include <boost/range/size.hpp>

#include <utility>

namespace paal {

namespace detail {

template <typename InputRow,
          typename DestRow,
          typename Dummy>
void copy_row_to_matrix(InputRow &&input_row, DestRow &dest_matrix_row,
                        Dummy /* is_row_of_sparse_matrix */,
                        std::true_type /* rows_are_assignable */) {
    assert(boost::size(dest_matrix_row) == boost::size(input_row));
    dest_matrix_row = input_row;
};

template <typename InputRow,
          typename DestRow>
void copy_row_to_matrix(InputRow &&input_row, DestRow &dest_matrix_row,
                        std::false_type /* is_row_of_sparse_matrix*/,
                        std::false_type /* rows_are_assignable */) {
    //boost::size does not compile on ublas structures
    assert(boost::size(dest_matrix_row) ==
           std::distance(std::begin(input_row),
                         std::end(input_row)));
    std::copy(std::begin(input_row),
              std::end(input_row),
              std::begin(dest_matrix_row));
};

template <typename InputRow,
          typename DestRow>
void copy_row_to_matrix(InputRow &&input_row, DestRow &dest_matrix_row,
                        std::true_type /* is_row_of_sparse_matrix */,
                        std::false_type /* rows_are_assignable */) {
    for (auto it = std::begin(input_row); it != std::end(input_row); ++it) {
        assert(it.index2() <= dest_matrix_row.size());
        dest_matrix_row(it.index2()) = *it;
    }
};


template <typename InputRow,
          typename DestRow>
void copy_row_to_matrix(InputRow &&input_row, DestRow &dest_matrix_row) {
    copy_row_to_matrix(std::forward<InputRow>(input_row), dest_matrix_row,
            typename data_structures::is_sparse_row<InputRow>::type(),
            typename std::is_assignable<DestRow,InputRow>::type());
};

} // detail

/**
 * @brief Represents sketch of matrix
 *
 * example:
 *  \snippet frequent_directions_example.cpp Frequent Directions Example
 *
 * complete example is frequent_directions_example.cpp
 * @tparam T
 * @tparam Matrix
 */
template <typename T,
          typename Matrix = boost::numeric::ublas::matrix<T>>
class frequent_directions {
    Matrix m_sketch;
    std::size_t m_actual_size;
    std::size_t m_compress_size;

    using matrix_types = data_structures::matrix_type_traits<Matrix>;

public:
    ///serialize
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & m_actual_size;
        ar & m_compress_size;
        ar & m_sketch;
    }

    /**
     * @brief Creates sketch.
     *
     * Default compress_size is half of number of rows in sketch matrix
     *
     * @param matrix sketch matrix where data is stored
     */
    frequent_directions(Matrix &&matrix)
      : frequent_directions(std::forward<Matrix>(matrix),
                            matrix_types::num_rows(matrix) / 2) {}

    /**
     * @brief Creates sketch.
     *
     * @param matrix sketch matrix where data is stored
     * @param compress_size number of nonzero rows after compress phase
     */
    frequent_directions(Matrix &&matrix, std::size_t const compress_size)
      : m_sketch(matrix), m_actual_size(0),
        m_compress_size(std::min(compress_size, matrix_types::num_columns(matrix))) {
          assert(m_compress_size >= 0);
          assert(m_compress_size < matrix_types::num_rows(m_sketch));
    }

    ///default constructor
    frequent_directions() : m_actual_size(0), m_compress_size(0) {}

    ///operator==
    bool operator==(frequent_directions const & other) const {
        return m_actual_size == other.m_actual_size &&
               m_compress_size == other.m_compress_size &&
               boost::numeric::ublas::detail::expression_type_check(m_sketch, other.m_sketch);
    }

    ///Adds new data in matrix form.
    template <typename MatrixData>
    void update(MatrixData&& matrix) {
        for (auto row = matrix.begin1(); row != matrix.end1(); ++row)
            update_row(std::move(row));
    }

    ///Adds one new row.
    template <typename InputRow>
    void update_row(InputRow&& input_row) {
        if(m_actual_size == matrix_types::num_rows(m_sketch))
        {
            compress();
        }
        typename matrix_types::matrix_row_t dest_row{m_sketch, m_actual_size++};
        detail::copy_row_to_matrix(std::forward<InputRow>(input_row), dest_row);
    }

    //TODO add number of threads
    ///Adds new rows.
    template <typename RowRange>
    void update_range(RowRange&& row_range) {
        for(auto row : row_range)
            update_row(row);
    }

    /**
     * @brief Compress sketch.
     *
     * After compress phase sketch contains m_compress_size nonzero rows.
     */
    void compress() {
        auto rows_count = matrix_types::num_rows(m_sketch);
        auto columns_count = matrix_types::num_columns(m_sketch);
        auto min_dimension = std::min(rows_count, columns_count);

        typename matrix_types::matrix_column_major_t u{rows_count,min_dimension}, vt{min_dimension,columns_count};
        typename matrix_types::vector_t sigma{min_dimension};

        typename matrix_types::matrix_column_major_t sketch{std::move(m_sketch)};
        boost::numeric::bindings::lapack::gesvd(sketch, sigma, u, vt);

        T const delta = m_compress_size < min_dimension ? sigma[m_compress_size] * sigma[m_compress_size] : T{};

        for (auto &sigma_i : sigma) {
            sigma_i = std::sqrt(std::max(sigma_i * sigma_i - delta, T{}));
        }

        typename matrix_types::matrix_diagonal_t s_diagonal{rows_count, min_dimension, 0, 0, std::move(sigma.data())};
        m_sketch = boost::numeric::ublas::prod(s_diagonal, vt);

        m_actual_size = m_compress_size;
    }

    /**
     * @brief
     *
     * @returns sketch and number of its nonzero rows
     */
    std::pair<Matrix const &, std::size_t> get_sketch() {
      return std::make_pair(std::cref(m_sketch), m_actual_size);
    }
};

} // paal

#endif // PAAL_FREQUENT_DIRECTIONS_HPP
