//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file knapsack_long_test.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-09-20
 */

#include "test_utils/logger.hpp"
#include "test_utils/read_knapsack.hpp"
#include "test_utils/knapsack_tags_utils.hpp"
#include "test_utils/parse_file.hpp"

#include "paal/dynamic/knapsack_unbounded.hpp"
#include "paal/dynamic/knapsack_0_1.hpp"
#include "paal/utils/floating.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/range/algorithm/random_shuffle.hpp>

#include <fstream>

using namespace paal;
using namespace paal::utils;

BOOST_AUTO_TEST_CASE(KnapSackLong) {
    std::string testDir = "test/data/KNAPSACK/";
    parse(testDir + "cases.txt",
          [&](const std::string & line, std::istream & is_test_cases) {
        int testId = std::stoi(line);
        LOGLN("test >>>>>>>>>>>>>>>>>>>>>>>>>>>> " << testId);

        int capacity;
        std::vector<std::pair<int, int>> objects;
        std::vector<int> optimal;

        auto size = [](std::pair<int, int> object) { return object.first; }
        ;
        auto value = [](std::pair<int, int> object) { return object.second; }
        ;

        read(testDir + "cases/", testId, capacity, objects, optimal);
        LOGLN("capacity " << capacity);
        LOGLN("sizes ");

        LOGLN("");
        LOGLN("size values");
        ON_LOG(for (auto o
                    : objects) {
            std::cout << "{ size = " << o.first << ", value = " << o.second
                      << "} ";
        });
        LOGLN("");
        LOGLN("Optimal 0/1");
        LOG_COPY_RANGE_DEL(optimal, " ");
        LOGLN("");
        auto opt_0_1 = boost::accumulate(optimal, 0, [&](int sum, int i) {
            return sum + objects[i].second;
        });
        ON_LOG(auto optSize = )
            boost::accumulate(optimal, 0, [&](int sum, int i) {
            return sum + objects[i].first;
        });
        LOGLN("Opt size " << optSize << " opt " << opt_0_1);
        LOGLN("");

        // KNAPSACK
        auto opt =
            detail_knapsack<pd::integral_value_and_size_tag, pd::unbounded_tag>(
                objects, capacity, size, value).first;

        boost::random_shuffle(objects);

        auto maxValue =
            detail_knapsack<pd::integral_value_tag, pd::unbounded_tag>(
                objects, capacity, size, value);

        boost::random_shuffle(objects);
        BOOST_CHECK_EQUAL(opt, maxValue.first);
        maxValue = detail_knapsack<pd::integral_size_tag, pd::unbounded_tag>(
            objects, capacity, size, value);
        boost::random_shuffle(objects);
        BOOST_CHECK_EQUAL(opt, maxValue.first);

        // KNAPSACK 0/1

        maxValue =
            detail_knapsack<pd::integral_value_and_size_tag, pd::zero_one_tag>(
                objects, capacity, size, value);
        boost::random_shuffle(objects);
        BOOST_CHECK_EQUAL(opt_0_1, maxValue.first);
        maxValue = detail_knapsack<pd::integral_size_tag, pd::zero_one_tag>(
            objects, capacity, size, value);
        boost::random_shuffle(objects);
        BOOST_CHECK_EQUAL(opt_0_1, maxValue.first);
        maxValue = detail_knapsack<pd::integral_value_tag, pd::zero_one_tag>(
            objects, capacity, size, value);
        boost::random_shuffle(objects);
        BOOST_CHECK_EQUAL(opt_0_1, maxValue.first);

        maxValue =
            detail_knapsack<pd::integral_value_and_size_tag, pd::zero_one_tag,
                            pd::no_retrieve_solution_tag>(objects, capacity,
                                                          size, value);
        boost::random_shuffle(objects);
        BOOST_CHECK_EQUAL(opt_0_1, maxValue.first);
        maxValue = detail_knapsack<pd::integral_size_tag, pd::zero_one_tag,
                                   pd::no_retrieve_solution_tag>(
            objects, capacity, size, value);
        boost::random_shuffle(objects);
        BOOST_CHECK_EQUAL(opt_0_1, maxValue.first);
        maxValue = detail_knapsack<pd::integral_value_tag, pd::zero_one_tag,
                                   pd::no_retrieve_solution_tag>(
            objects, capacity, size, value);
        boost::random_shuffle(objects);
        BOOST_CHECK_EQUAL(opt_0_1, maxValue.first);

        // FPTAS
        auto epsilons = { 0.00001, 0.0001, 0.001, 0.01, 0.1, 0.2, 0.3, 0.4, 0.5,
                          0.6, 0.7 };

        for (auto epsilon : epsilons) {
            // KNAPSACK unbounded on value
            maxValue = detail_knapsack_fptas<pd::unbounded_tag,
                                             pd::retrieve_solution_tag>(
                epsilon, objects, capacity, size, value, on_value_tag{});
            boost::random_shuffle(objects);
            BOOST_CHECK(double(opt) * (1. - epsilon) <= maxValue.first);
            BOOST_CHECK(capacity >= maxValue.second);

            // KNAPSACK unbounded on size
            // this might possibly fail since this version is not really a FPTAS
            maxValue = detail_knapsack_fptas<pd::unbounded_tag,
                                             pd::retrieve_solution_tag>(
                epsilon, objects, capacity, size, value, on_size_tag{});
            boost::random_shuffle(objects);
            BOOST_CHECK(opt <= maxValue.first);
            BOOST_CHECK(double(capacity) * (1. + epsilon) >= maxValue.second);

            // KNAPSACK 0_1 on value
            maxValue = detail_knapsack_fptas<pd::zero_one_tag,
                                             pd::retrieve_solution_tag>(
                epsilon, objects, capacity, size, value, on_value_tag{});
            boost::random_shuffle(objects);
            BOOST_CHECK(double(opt_0_1) * (1. - epsilon) <= maxValue.first);
            BOOST_CHECK(capacity >= maxValue.second);

            // KNAPSACK 0_1 on size
            maxValue = detail_knapsack_fptas<pd::zero_one_tag,
                                             pd::retrieve_solution_tag>(
                epsilon, objects, capacity, size, value, on_size_tag{});
            boost::random_shuffle(objects);
            BOOST_CHECK(opt_0_1 <= maxValue.first);
            BOOST_CHECK(double(capacity) * (1. + epsilon) >= maxValue.second);
        }
    });
}
