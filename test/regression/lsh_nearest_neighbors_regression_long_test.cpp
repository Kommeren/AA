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
#include "paal/utils/functors.hpp"
#include "paal/utils/irange.hpp"
#include "paal/utils/log_loss.hpp"
#include "paal/utils/parse_file.hpp"
#include "paal/utils/read_svm.hpp"

#include <boost/mpl/list.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_sparse.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/fill.hpp>
#include <boost/range/empty.hpp>
#include <boost/range/size.hpp>
#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <istream>
#include <string>
#include <utility>
#include <vector>

namespace {

template<typename FloatType = double,
         typename TrainResults, typename TestResults>
FloatType train_avg_const_result_log_loss(TrainResults &&train_results,
                                          TestResults &&test_results) {
    assert(!boost::empty(train_results));
    assert(!boost::empty(test_results));

    FloatType train_avg = boost::accumulate(train_results, FloatType{}) /
        boost::size(train_results);
    assert(0.0 < train_avg);
    assert(train_avg < 1.0);

    auto avg_const_iterator = paal::irange(boost::size(test_results)) |
        boost::adaptors::transformed(
                paal::utils::make_dynamic_return_constant_functor(train_avg));
    return paal::log_loss<FloatType>(avg_const_iterator, test_results);
}

template <typename ResultType = double,
          typename PointWithResultType,
          typename HashFunctionGenerator>
bool beats_average(const std::vector<PointWithResultType> &train_points,
                   const std::vector<PointWithResultType> &test_points,
                   unsigned passes,
                   unsigned hash_funs_per_row,
                   HashFunctionGenerator hash_function_generator) {
    assert(!boost::empty(train_points));
    constexpr paal::utils::tuple_get<0> get_coordinates{};
    constexpr paal::utils::tuple_get<1> get_result{};

    using boost::adaptors::transformed;
    auto const train_points_coordinates =
        train_points | transformed(get_coordinates);
    auto const train_points_results =
        train_points | transformed(get_result);
    auto const test_points_coordinates =
        test_points | transformed(get_coordinates);
    auto const test_points_results =
        test_points | transformed(get_result);

    auto model = paal::make_lsh_nearest_neighbors_regression_tuple_hash(
            train_points_coordinates,
            train_points_results,
            passes,
            std::move(hash_function_generator),
            hash_funs_per_row);

    std::vector<ResultType> alg_results(test_points.size());
    model.test(test_points_coordinates, alg_results.begin());

    auto alg_results_log_loss = paal::log_loss<ResultType>(alg_results,
                                                           test_points_results);
    auto benchmark = train_avg_const_result_log_loss(train_points_results,
                                                     test_points_results);
    auto benchmark_ratio = 1.0 - 1e-5;
    LOGLN("log loss: " << alg_results_log_loss << " benchmark: " << benchmark);
    return alg_results_log_loss / benchmark < benchmark_ratio;
}

template <typename ResultType = double,
          typename PointWithResultType>
bool beats_average(const std::vector<PointWithResultType> &train_points,
                   const std::vector<PointWithResultType> &test_points,
                   std::size_t dimensions) {
    for (auto passes : {100, 500, 1000}) {
        std::size_t max_hash_funs = std::ceil(std::sqrt(dimensions));
        for (auto hash_funs_per_row : paal::irange(1ul, max_hash_funs)) {
            //TODO compute w_param based on train_points
            auto w_param = 10000.0;
            LOGLN("passes: " << passes <<
                  " hash_funs_per_row: " << hash_funs_per_row <<
                  " w_param: " << w_param);

            using l_1_hash_gen = paal::hash::l_1_hash_function_generator<>;
            using l_2_hash_gen = paal::hash::l_2_hash_function_generator<>;
            using hamming_gen = paal::hash::hamming_hash_function_generator;

            LOGLN("Checking l_1:");
            if (beats_average(train_points, test_points,
                              passes, hash_funs_per_row,
                              l_1_hash_gen{dimensions, w_param})) {
                return true;
            }

            LOGLN("Checking l_2:");
            if (beats_average(train_points, test_points,
                              passes, hash_funs_per_row,
                              l_2_hash_gen{dimensions, w_param})) {
                return true;
            }

            LOGLN("Checking Hamming:");
            if (beats_average(train_points, test_points,
                              passes, hash_funs_per_row,
                              hamming_gen{dimensions})) {
                return true;
            }
        }
    }
    return false;
}


} //! unnamed

BOOST_AUTO_TEST_SUITE(lsh_nearest_neighbors_regression_long)

using coordinate_t = double;
typedef boost::mpl::list<
    boost::numeric::ublas::vector<coordinate_t>,
    boost::numeric::ublas::mapped_vector<coordinate_t>,
    boost::numeric::ublas::compressed_vector<coordinate_t>,
    boost::numeric::ublas::coordinate_vector<coordinate_t>
    > test_vector_types;

BOOST_AUTO_TEST_CASE_TEMPLATE(l1_l2_regression, VectorType, test_vector_types) {
    std::string testDir = "test/data/LSH_REGRESSION/";

    paal::parse(testDir + "cases.txt",
                [&](const std::string &file_name, paal::utils::ignore_param) {
        LOGLN("TEST " << file_name);
        std::string train_file_path = testDir + "cases/" + file_name;
        std::ifstream train_file_stream(train_file_path);
        std::ifstream test_file_stream(train_file_path + ".t");

        auto train_points = paal::read_svm<VectorType>(train_file_stream);
        auto test_points = paal::read_svm<VectorType>(test_file_stream);

        using std::get;
        using paal::utils::tuple_get;
        auto dimensions = std::max(get<1>(train_points), get<1>(test_points));

        LOGLN("Reading data");
        using paal::detail::resize_rows;
        resize_rows(get<0>(train_points), tuple_get<0>(), dimensions);
        resize_rows(get<0>(test_points), tuple_get<0>(), dimensions);

        BOOST_CHECK_MESSAGE(beats_average(get<0>(train_points),
                                          get<0>(test_points),
                                          dimensions),
                            "Failed to beat average for case:" << file_name);
    });
}

BOOST_AUTO_TEST_SUITE_END()

