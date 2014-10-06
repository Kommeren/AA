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
 * @brief Function parses svm stream of format:
 * line .=. result feature:value feature:value ... feature:value
 * result .=. +1 | -1
 * feature .=. positive integer
 * value .=. float
 *
 * Empty lines or lines beginning with '#' are ignored.
 * Results are converted to 0.0 and 1.0.
 * Feature ids are substracted by 1 in order to begin with 0.
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
          typename ResultType = double>
auto read_svm(std::istream &input_stream) {
    using coordinate_t = range_to_elem_t<RowType>;
    using point_with_result_t = std::tuple<RowType, ResultType>;

    unsigned max_feature_id = 0;
    std::vector<point_with_result_t> points;
    paal::parse(input_stream,
                [&](std::string &result_string, std::istream &input_stream) {
        ResultType result = (std::stoi(result_string) == 1) ? 1.0 : 0.0;

        std::string coordinates_line;
        std::getline(input_stream, coordinates_line);
        std::stringstream point_stream(coordinates_line);

        RowType coordinates;
        while (point_stream.good()) {
            unsigned feature_id;
            coordinate_t coordinate;
            point_stream >> feature_id;

            point_stream.ignore(1, ':');
            point_stream >> coordinate;

            assert(feature_id > 0);
            assign_max(max_feature_id, feature_id);
            if (coordinates.size() < feature_id) {
                coordinates.resize(max_feature_id);
            }
            coordinates[feature_id - 1] = coordinate;
        }
        points.push_back(std::make_tuple(coordinates, result));
    });
    detail::resize_rows(points, utils::tuple_get<0>(), max_feature_id);

    return std::make_tuple(points, max_feature_id);
}

} //! paal

#endif /* PALL_READ_SVM_HPP */

