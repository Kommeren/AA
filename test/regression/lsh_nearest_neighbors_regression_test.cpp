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
#include "test_utils/get_test_dir.hpp"

#include "paal/regression/lsh_nearest_neighbors_regression.hpp"
#include "paal/utils/irange.hpp"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/numeric/ublas/vector_sparse.hpp>
#include <boost/range/empty.hpp>
#include <boost/test/unit_test.hpp>

#include <iterator>
#include <vector>
#include <utility>

namespace {

using coordinate_t = int;
using result_t = double;
using point_coordinates_t = std::vector<coordinate_t>;
using point_sparse_coordinates_t = boost::numeric::ublas::compressed_vector<double>;

static result_t const EPSILON = 1e-9;
static unsigned const default_passes = 50;
static unsigned const default_hash_functions_per_row = 2;

struct jaccard_tag{};
struct hamming_tag{};

auto make_model(hamming_tag, std::vector<point_coordinates_t> const &training_points,
                std::vector<result_t> const &training_results,
                unsigned passes = default_passes,
                unsigned hash_functions_per_row
                    = default_hash_functions_per_row) {
    auto const dimensions = training_points.front().size();
    return paal::make_lsh_nearest_neighbors_regression_tuple_hash(
                training_points,
                training_results,
                passes,
                paal::lsh::hamming_hash_function_generator{dimensions},
                hash_functions_per_row);
}

auto make_model(jaccard_tag, std::vector<point_sparse_coordinates_t> const &training_points,
                std::vector<result_t> const &training_results,
                unsigned passes = default_passes,
                unsigned hash_functions_per_row
                    = default_hash_functions_per_row) {
    auto const dimensions = training_points.front().size();
    return paal::make_lsh_nearest_neighbors_regression_tuple_hash(
                training_points,
                training_results,
                passes,
                paal::lsh::min_hash_function_generator<>{dimensions},
                hash_functions_per_row);
}

template <typename HashFunctionGenerator>
auto make_lp(std::vector<point_sparse_coordinates_t> const &training_points,
             std::vector<result_t> const &training_results,
             unsigned passes = default_passes,
             unsigned hash_functions_per_row
                 = default_hash_functions_per_row) {
    auto const dimensions = training_points.front().size();
    int const W = 2;
    return paal::make_lsh_nearest_neighbors_regression_tuple_hash(
                training_points,
                training_results,
                passes,
                HashFunctionGenerator{dimensions, W},
                hash_functions_per_row);
}

template<typename Model>
void update(Model &&model,
            std::vector<point_coordinates_t> const &training_points,
            std::vector<result_t> const &training_results) {
    model.update(training_points, training_results);
}

template <typename Model, typename Row>
void test(Model &&model,
          std::vector<Row> const &test_points,
          std::vector<result_t> const &expected_results) {
    std::vector<result_t> results(test_points.size());
    model.test(test_points, results.begin());

    LOGLN("expected results");
    LOG_COPY_RANGE_DEL(expected_results, ",");
    LOGLN("");

    LOGLN("results");
    LOG_COPY_RANGE_DEL(results, ",");
    LOGLN("");

    for (auto i : paal::irange(test_points.size())) {
        BOOST_CHECK_CLOSE(expected_results[i], results[i], EPSILON);
    }
}

template <typename HashFunctionTag, typename Row>
void check_hamming_jaccard(HashFunctionTag tag, std::vector<Row> const &training_points,
                   std::vector<result_t> const &training_results,
                   std::vector<Row> const &test_points,
                   std::vector<result_t> const &expected_results,
                   unsigned passes = default_passes,
                   unsigned hash_functions_per_row
                       = default_hash_functions_per_row) {
    auto model = make_model(tag, training_points, training_results,
                                    passes, hash_functions_per_row);
    test(std::move(model), test_points, expected_results);
}

void check_hamming(std::vector<point_coordinates_t> const &training_points,
                   std::vector<result_t> const &training_results,
                   std::vector<point_coordinates_t> const &test_points,
                   std::vector<result_t> const &expected_results,
                   unsigned passes = default_passes,
                   unsigned hash_functions_per_row
                       = default_hash_functions_per_row) {
    check_hamming_jaccard(hamming_tag{}, training_points, training_results, test_points,
            expected_results, passes, hash_functions_per_row);
}

point_sparse_coordinates_t make_sparse_set_from_coordinates(std::vector<int> const &coordinates) {
    assert(!boost::empty(coordinates));
    point_sparse_coordinates_t point(*boost::max_element(coordinates) + 1);
    for (auto index : coordinates) point(index) = 1.0;
    return point;
};

std::vector<point_sparse_coordinates_t> make_jaccard_points(std::vector<point_coordinates_t> const &points) {
    std::vector<point_sparse_coordinates_t> converted;
    size_t max_size = 0;
    for (auto &coordinates : points) {
        auto converted_point = make_sparse_set_from_coordinates(coordinates);
        max_size = std::max(max_size, converted_point.size());
        converted.push_back(std::move(converted_point));
    }

    for (auto &point : converted) {
        point.resize(max_size);
    }
    return converted;
}


auto make_sparse(std::vector<double> const &list) {
    point_sparse_coordinates_t point(list.size());
    for (auto index : paal::irange(list.size())) {
        point[index] = list[index];
    }
    return point;
};

} //! unnamed

BOOST_AUTO_TEST_SUITE(lsh_nearest_neighbors_regression)

BOOST_AUTO_TEST_CASE(training_points_same_as_test_points) {
    // the more hash functions, the lower chance of accidental positive
    LOGLN("training_points_same_as_test_points");
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
    //test always passes, since in case of missing all training points hash values
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
                  //0.0-result training point and 1.0-result for second point
                  {{0, -1}, {2, 1}},
                  {0.0, 1.0});
}

BOOST_AUTO_TEST_CASE(two_descent_values_ints_results) {
    //test passes with probability 1 - 0.5**50
    LOGLN("two_descent_values_ints_results");
    check_hamming({{0, 0}, {3, -1}, {0, 2}, {4, 1}, {5, 1}, {2, 5}},
                  {0, 0, 0, 1, 1, 1},
                  //first point projections matches projections of
                  //0.0-result training point and 1.0-result for second point
                  {{0, -1}, {2, 1}},
                  {0, 1});
}

BOOST_AUTO_TEST_CASE(two_descent_values_ints_results_min) {
    LOGLN("two_descent_values_ints_results min hashing");
    check_hamming_jaccard(jaccard_tag{},
                make_jaccard_points({{0, 1}, {1}, {0}, {2, 3}, {2}, {3}}),
                {0, 0, 0, 1, 1, 1},
                make_jaccard_points({{0, 1}, {3, 2}}),
                {0, 1});
}


BOOST_AUTO_TEST_CASE(no_neighbors_return_average) {
    LOGLN("no_neighbors_return_average");
    check_hamming({{0, 0}, {3, -1}, {0, 2}, {4, 1}, {5, 1}, {2, 5}},
                  {0.0, 0.0, 0.0, 1.0, 1.0, 1.0},
                  //coordinates different from training points coordinates
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
    auto model = make_model(hamming_tag{}, {{0, 1}, {2, 3}},
                                    {0.0, 0.2});
    //point distant from training points
    LOGLN("test");
    std::vector<point_coordinates_t> test_points = {{17, 18}};
    test(model, test_points, {0.1});
    LOGLN("update");
    update(model, {{4, 5}}, {0.4});
    LOGLN("test");
    test(model, test_points, {0.2});
    LOGLN("end");
}

template <typename Model>
void serialize(Model const &  model) {
    auto fname = paal::system::get_temp_file_path("tmp.bin");

    {
        std::ofstream ofs(fname);
        boost::archive::binary_oarchive oa(ofs);
        oa << model;
    }

    Model model_test;
    std::ifstream ifs(fname);
    boost::archive::binary_iarchive ia(ifs);
    ia >> model_test;

    BOOST_CHECK(model == model_test);
    paal::system::remove_tmp_path(fname);
}

BOOST_AUTO_TEST_CASE(serialization) {
    LOGLN("serialize");
    serialize(make_model(hamming_tag{}, {{0, 1}, {2, 3}}, {0.0, 0.2}));
    serialize(make_model(jaccard_tag{},
                         make_jaccard_points({{0, 1}, {2, 3}}),
                         {0.0, 0.2}));

    point_sparse_coordinates_t p1{make_sparse({0.0, 1.0})}, p2{make_sparse({2.0, 3.0})};
    serialize(make_lp<paal::lsh::l_1_hash_function_generator<>>({p1, p2}, {0.0, 0.2}));
    serialize(make_lp<paal::lsh::l_2_hash_function_generator<>>({p1, p2}, {0.0, 0.2}));
}

BOOST_AUTO_TEST_SUITE_END()

