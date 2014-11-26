//=======================================================================
// Copyright (c) 2014 Andrzej Pacuk
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file lsh_nearest_neighbors_regression_test.cpp
 * @brief
 * @author Andrzej Pacuk
 * @version 1.0
 * @date 2014-10-07
 */
#include "test_utils/logger.hpp"

#include "paal/regression/lsh_nearest_neighbors_regression.hpp"

#include "paal/utils/hash_functions.hpp"
#include "paal/utils/irange.hpp"

#include <boost/test/unit_test.hpp>

#include <iterator>
#include <vector>
#include <utility>

namespace {

using coordinate_t = int;
using result_t = double;
using point_coordinates_t = std::vector<coordinate_t>;

static const result_t EPSILON = 1e-9;
static const unsigned default_passes = 50;
static const unsigned default_hash_functions_per_row = 2;

auto make_hamming_model(const std::vector<point_coordinates_t> &train_points,
                        const std::vector<result_t> &train_results,
                        unsigned passes = default_passes,
                        unsigned hash_functions_per_row
                            = default_hash_functions_per_row) {
    auto const dimensions = train_points.front().size();
    return paal::make_lsh_nearest_neighbors_regression_tuple_hash(
                train_points,
                train_results,
                passes,
                paal::hash::hamming_hash_function_generator{dimensions},
                hash_functions_per_row);
}

template<typename Model>
void update(Model &&model,
            const std::vector<point_coordinates_t> &train_points,
            const std::vector<result_t> &train_results) {
    model.update(train_points, train_results);
}

template <typename Model>
void test(Model &&model,
          const std::vector<point_coordinates_t> &query_points,
          const std::vector<result_t> &expected_results) {
    std::vector<result_t> results(query_points.size());
    model.test(query_points, results.begin());

    LOGLN("expected results");
    LOG_COPY_RANGE_DEL(expected_results, ",");
    LOGLN("");

    LOGLN("results");
    LOG_COPY_RANGE_DEL(results, ",");
    LOGLN("");

    for (auto i : paal::irange(query_points.size())) {
        BOOST_CHECK_CLOSE(expected_results[i], results[i], EPSILON);
    }
}


void check_hamming(const std::vector<point_coordinates_t> &train_points,
                   const std::vector<result_t> &train_results,
                   const std::vector<point_coordinates_t> &query_points,
                   const std::vector<result_t> &expected_results,
                   unsigned passes = default_passes,
                   unsigned hash_functions_per_row
                       = default_hash_functions_per_row) {
    auto model = make_hamming_model(train_points, train_results,
                                    passes, hash_functions_per_row);
    test(std::move(model), query_points, expected_results);
}

} //! unnamed

BOOST_AUTO_TEST_SUITE(lsh_nearest_neighbors_regression_hamming)

BOOST_AUTO_TEST_CASE(train_points_same_as_test_points) {
    // the more hash functions, the lower chance of accidental positive
    LOGLN("train_points_same_as_test_points");
    auto hash_functions_per_row = 50;
    for (auto passes : {1, 10, 50}) {
        check_hamming({{0, 0}, {1, 1}},
                      {0.0, 1.0},
                      {{0, 0}, {1, 1}},
                      {0.0, 1.0},
                      passes,
                      hash_functions_per_row);
    }
}

BOOST_AUTO_TEST_CASE(one_value) {
    //test always passes, since in case of missing all train points hash values
    //still average = 1.0
    LOGLN("one_value");
    check_hamming({{0, 0}, {3, -1}, {0, 2}, {4, 1}, {5, 1}, {2, 5}},
                  {1.0, 1.0, 1.0, 1.0, 1.0, 1.0},
                  {{0, -1}},
                  {1.0});
}

BOOST_AUTO_TEST_CASE(two_descent_values) {
    //test passes with probability 1 - 0.5**50
    LOGLN("two_descent_values");
    check_hamming({{0, 0}, {3, -1}, {0, 2}, {4, 1}, {5, 1}, {2, 5}},
                  {0.0, 0.0, 0.0, 1.0, 1.0, 1.0},
                  //first point projections matches projections of
                  //0.0-result train point and 1.0-result for second point
                  {{0, -1}, {2, 1}},
                  {0.0, 1.0});
}

BOOST_AUTO_TEST_CASE(two_descent_values_ints_results) {
    //test passes with probability 1 - 0.5**50
    LOGLN("two_descent_values_ints_results");
    check_hamming({{0, 0}, {3, -1}, {0, 2}, {4, 1}, {5, 1}, {2, 5}},
                  {0, 0, 0, 1, 1, 1},
                  //first point projections matches projections of
                  //0.0-result train point and 1.0-result for second point
                  {{0, -1}, {2, 1}},
                  {0, 1});
}

BOOST_AUTO_TEST_CASE(no_neighbors_return_average) {
    LOGLN("no_neighbors_return_average");
    check_hamming({{0, 0}, {3, -1}, {0, 2}, {4, 1}, {5, 1}, {2, 5}},
                  {0.0, 0.0, 0.0, 1.0, 1.0, 1.0},
                  //coordinates different from train points coordinates
                  {{14, -7}},
                  {0.5});
}

BOOST_AUTO_TEST_CASE(no_hit_return_average) {
    LOGLN("no_hit_return_average");
    using puu = std::pair<unsigned, unsigned>;
    for (auto const &params : {puu(0, 50), puu(50, 0), puu(0, 0)}) {
        auto passes = params.first;
        auto hash_functions_per_row = params.second;
        LOGLN("passes: " << passes);
        LOGLN("hash_functions_per_row: " << hash_functions_per_row);
        check_hamming({{0, 0}, {3, -1}},
                      {0.0, 1.0},
                      {{14, -7}},
                      {0.5},
                      passes,
                      hash_functions_per_row);
    }
}

BOOST_AUTO_TEST_CASE(update_changes_model) {
    LOGLN("update_changes_model");
    auto model = make_hamming_model({{0, 1}, {2, 3}},
                                    {0.0, 0.2});
    //point distant from train points
    LOGLN("test");
    test(model, {{17, 18}}, {0.1});
    LOGLN("update");
    update(model, {{4, 5}}, {0.4});
    LOGLN("test");
    test(model, {{17, 18}}, {0.2});
    LOGLN("end");
}


BOOST_AUTO_TEST_SUITE_END()

