//=======================================================================
// Copyright (c) 2014 Piotr Wygocki
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file lsh_nearest_neighbors_regression_efficiency.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2014-11-27
 */

#include "paal/regression/lsh_nearest_neighbors_regression.hpp"
#include "paal/utils/functors.hpp"

#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/irange.hpp>

#include <iostream>
#include <iterator>
#include <utility>
#include <vector>
#include <random>

int main() {
    using coordinate_t = int;
    using result_t = double;
    using point_coordinates_t = std::vector<coordinate_t>;
    using training_point_t = std::tuple<point_coordinates_t, result_t>;

    constexpr paal::utils::tuple_get<0> get_coordinates{};
    constexpr paal::utils::tuple_get<1> get_result{};

    auto const COORDINATE_NR = 10;
    auto const POINTS_NR = int(1e5);

    std::vector<training_point_t> training_points(POINTS_NR,
            training_point_t{point_coordinates_t(COORDINATE_NR), 0.0});
    std::vector<point_coordinates_t> test_points(POINTS_NR, point_coordinates_t(COORDINATE_NR));

    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(0,1);

    for (auto & p : training_points) {
        for (auto & c : std::get<0>(p)) {
            c = distribution(generator);
        }
    }

    auto const dimensions = std::get<0>(training_points.front()).size();
    auto const PASSES = 50;
    auto const HASH_FUNCTIONS_PER_POINT = 4;
    auto const THREADS_COUNT = 2;
    auto const REPEAT_NR = 10;

    for (auto i = 0; i < REPEAT_NR; ++i) {
        auto model = paal::make_lsh_nearest_neighbors_regression_tuple_hash(
                training_points | boost::adaptors::transformed(get_coordinates),
                training_points | boost::adaptors::transformed(get_result),
                PASSES,
                paal::lsh::hamming_hash_function_generator{dimensions},
                HASH_FUNCTIONS_PER_POINT, THREADS_COUNT);

        std::vector<result_t> results;
        results.reserve(test_points.size());
        model.test(test_points, std::back_inserter(results));
    }


    return 0;
}

