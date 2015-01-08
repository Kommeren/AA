//=======================================================================
// Copyright (c) 2014 Andrzej Pacuk
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file read_svm.hpp
 * @brief
 * @author Andrzej Pacuk
 * @version 1.0
 * @date 2014-10-22
 */
#ifndef PALL_READ_SVM_HPP
#define PALL_READ_SVM_HPP

#include "paal/utils/assign_updates.hpp"
#include "paal/utils/functors.hpp"
#include "paal/utils/parse_file.hpp"
#include "paal/utils/type_functions.hpp"

#include <algorithm>
#include <cassert>
#include <istream>
#include <string>
#include <sstream>
#include <utility>
#include <vector>

namespace paal {

namespace detail {
/**
 * @brief resize rows to have equal sizes
 *
 * @tparam RowsRange
 * @tparam RowRefExtractor
 * @param rows
 * @param row_ref_extractor
 * @param new_size
 */
template <typename RowsRange, typename RowRefExtractor>
void resize_rows(RowsRange &&rows, RowRefExtractor row_ref_extractor,
                 std::size_t new_size) {
    for (auto &row : rows) {
        row_ref_extractor(row).resize(new_size);
    }
}

} //! detail


/**
 * @brief class that can read single svm row
 *
 * @tparam CoordinatesType
 * @tparam ResultType
 * @tparam FeatureIdType
 */
template <typename CoordinatesType,
          typename ResultType = int,
          typename FeatureIdType = std::size_t>
class svm_row {
    CoordinatesType m_coordinates;
    ResultType m_result;

public:
    ///constructor
    svm_row(FeatureIdType dimensions = 1) : m_coordinates(dimensions) {}

    /**
     * @brief reads svm row of format:
     * line .=. result feature:value feature:value ... feature:value
     * result .=. 1 | -1
     * feature .=. positive integer
     * value .=. float
     *
     * Results are converted to 0.0 and 1.0.
     * Feature ids are substracted by 1 in order to begin with 0.
     * This class should get as stream only row (If you'll give
     * stream it will read to many numbers!)
     *
     * @param row_stream - it should contain only one line!
     * @param row - created row from scratch
     *
     * @return should return empty stream
     */
    friend std::istream &operator>>(std::istream &row_stream, svm_row &row) {

        int result;
        row_stream >> result;
        row.m_result = (result == 1) ? 1 : 0;

        row.m_coordinates.clear();
        while (row_stream.good()) {
            FeatureIdType feature_id;
            row_stream >> feature_id;

            using coordinate_t = range_to_elem_t<CoordinatesType>;
            coordinate_t coordinate;

            row_stream.ignore(1, ':');
            row_stream >> coordinate;

            if (row.m_coordinates.size() < feature_id) {
                row.m_coordinates.resize(feature_id);
            }
            assert(feature_id > 0);
            row.m_coordinates[feature_id - 1] = coordinate;
        }

        return row_stream;
    }

    /// coordinates getter
    CoordinatesType const &get_coordinates() const { return m_coordinates; }
    /// result getter
    ResultType const &get_result() const { return m_result; }
};

/**
 * @brief reads up to max_points_to_read svm rows,
 * updating max_feature_id on each row
 *
 * @tparam RowType
 * @tparam ResultType
 * @param input_stream
 * @param max_feature_id
 * @param points
 * @param max_points_to_read
 */
template <typename RowType,
          typename ResultType = int>
void read_svm(std::istream &input_stream,
              std::size_t &max_feature_id,
              std::vector<std::tuple<RowType, ResultType>> &points,
              std::size_t max_points_to_read) {
    svm_row<RowType, ResultType> row{max_feature_id};
    std::string line;
    while ((max_points_to_read--) && std::getline(input_stream, line)) {
        std::stringstream row_stream(line);
        row_stream >> row;
        assign_max(max_feature_id, row.get_coordinates().size());
        points.emplace_back(row.get_coordinates(),
                            row.get_result());
    }
}


/**
 * @brief Function parses svm stream of format:
 *
 * @tparam RowType
 * @tparam ResultType
 * @tparam RowTypeTag
 * @param input_stream
 *
 * @return vector of tuples (point, max_feature_id), where each point is
 * tuple (RowType, result)
 */
template <typename RowType,
          typename ResultType = int>
auto read_svm(std::istream &input_stream) {
    using point_with_result_t = std::tuple<RowType, ResultType>;

    std::size_t max_feature_id = 0;
    std::vector<point_with_result_t> points;
    while (input_stream.good()) {
        read_svm(input_stream, max_feature_id, points, 1);
    }
    detail::resize_rows(points, utils::tuple_get<0>(), max_feature_id);

    return std::make_tuple(points, max_feature_id);
}

} //! paal

#endif /* PALL_READ_SVM_HPP */

