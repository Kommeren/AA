//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file knapsack_two_app_long_test.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-09-20
 */

#define LOGGER_ON

#include "utils/logger.hpp"
#include "utils/read_knapsack.hpp"
#include "utils/parse_file.hpp"
#include "utils/test_result_check.hpp"

#include "paal/greedy/knapsack_0_1_two_app.hpp"
#include "paal/greedy/knapsack_unbounded_two_app.hpp"
#include "paal/utils/floating.hpp"

#include <boost/test/unit_test.hpp>

#include <fstream>

using namespace paal;
using namespace paal::utils;

BOOST_AUTO_TEST_CASE(KnapsackTwoAppLong) {
    std::string testDir = "test/data/KNAPSACK/";

    parse(testDir + "cases.txt", [&](const std::string & line, std::istream &) {
        int testId = std::stoi(line);
        LOGLN("test >>>>>>>>>>>>>>>>>>>>>>>>>>>> " << testId);

        int capacity;
        std::vector<std::pair<int, int>> sizes_values;
        std::vector<int> optimal;
        auto size = [](std::pair<int, int> object) { return object.first; }
        ;
        auto value = [](std::pair<int, int> object) { return object.second; }
        ;

        read(testDir + "cases/", testId, capacity, sizes_values, optimal);
        LOGLN("capacity " << capacity);
        //      LOGLN("size - value pairs ");
        //        LOG_COPY_RANGE_DEL(sizes_values, " ");
        LOGLN("");
        // KNAPSACK 0/1
        {
            std::vector<std::pair<int, int>> result;
            auto value_sum = [&](int sum, int i) {
                return sum + sizes_values[i].second;
            }
            ;
            auto opt = boost::accumulate(optimal, 0, value_sum);
            LOGLN("Knapsack 0/1");
            auto maxValue =
                knapsack_0_1_two_app(sizes_values, capacity,
                                     std::back_inserter(result), value, size);

            LOGLN("Max value " << maxValue.first << ", Total size "
                               << maxValue.second);
            //       LOG_COPY_RANGE_DEL(result, " ");
            LOGLN("");
            LOGLN("OPT");
            LOG_COPY_RANGE_DEL(optimal, " ");
            LOGLN("");

            BOOST_CHECK(maxValue.second <= capacity);
            check_result(maxValue.first, opt, 0.5, greater_equal{});
        }

        // KNAPSACK
        {
            std::vector<std::pair<int, int>> result;
            LOGLN("Knapsack unbounded");
            auto maxValue = knapsack_unbounded_two_app(
                sizes_values, capacity, std::back_inserter(result), value,
                size);

            LOGLN("Max value " << maxValue.first << ", Total size "
                               << maxValue.second);
            //         LOG_COPY_RANGE_DEL(result, " ");
            LOGLN("");
            BOOST_CHECK(maxValue.second <= capacity);
        }
    });
}

#include <iostream>

BOOST_AUTO_TEST_CASE(KnapsackTwoAppLongPerf) {
    std::srand(42);
    const int OBJECTS_NR = static_cast<int>(1e7);
    const int MAX = static_cast<int>(1e2);

    using Objects = std::vector<std::pair<int, int>>;
    Objects objects(OBJECTS_NR);

    auto rand_from_range = [ = ]() { return (std::rand() % MAX) + 1; }
    ;
    auto rand_pair = [ = ]() {
        return std::make_pair(rand_from_range(), rand_from_range());
    }
    ;
    std::generate_n(objects.begin(), OBJECTS_NR, rand_pair);

    auto total_size =
        boost::accumulate(objects, 0, [&](int sum, std::pair<int, int> p) {
        return sum + p.first;
    });

    auto size = [](std::pair<int, int> object) { return object.first; }
    ;
    auto value = [](std::pair<int, int> object) { return object.second; }
    ;

    auto capacity = total_size / 2;
    LOGLN("capacity = " << capacity);

    // KNAPSACK
    {
        Objects result;
        LOGLN("Knapsack unbounded");
        auto maxValue = knapsack_unbounded_two_app(
            objects, capacity, std::back_inserter(result), value, size);

        LOGLN("Max value " << maxValue.first << ", Total size "
                           << maxValue.second);
        BOOST_CHECK(maxValue.second <= capacity);
    }

    // KNAPSACK 0/1
    {
        Objects result;
        LOGLN("Knapsack 0/1");
        auto maxValue = knapsack_0_1_two_app(
            objects, capacity, std::back_inserter(result), value, size);

        LOGLN("Max value " << maxValue.first << ", Total size "
                           << maxValue.second);
        BOOST_CHECK(maxValue.second <= capacity);
    }
}
