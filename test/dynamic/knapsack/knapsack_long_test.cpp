/**
 * @file knapsack_long_test.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-09-20
 */

#include <fstream>

#include <boost/test/unit_test.hpp>
#include <boost/range/irange.hpp>

#include "paal/dynamic/knapsack.hpp"
#include "paal/dynamic/knapsack_0_1.hpp"
#include "paal/utils/floating.hpp"

#include "utils/logger.hpp"
#include "utils/read_knapsack.hpp"
#include "utils/knapsack_tags_utils.hpp"
#include "utils/parse_file.hpp"

using namespace paal;
using namespace paal::utils;


BOOST_AUTO_TEST_CASE(KnapSackLong) {
    std::string testDir = "test/data/KNAPSACK/";
    parse(testDir + "cases.txt", [&](const std::string & line, std::istream & is_test_cases) {
        int testId = std::stoi(line);
        LOGLN("test >>>>>>>>>>>>>>>>>>>>>>>>>>>> " << testId);

        int capacity;
        std::vector<int> sizes;
        std::vector<int> values;
        std::vector<int> optimal;

        auto sizesFunct = make_array_to_functor(sizes);
        auto valuesFunct = make_array_to_functor(values);

        read(testDir + "cases/", testId, capacity, sizes, values, optimal);
        LOGLN("capacity " << capacity);
        LOGLN("sizes ");

        LOG_COPY_RANGE_DEL(sizes, " ");
        LOGLN("");
        LOGLN("values ");
        LOG_COPY_RANGE_DEL(values, " ");
        LOGLN("");
        LOGLN("Optimal 0/1");
        LOG_COPY_RANGE_DEL(optimal, " ");
        LOGLN("");
        auto opt_0_1 = std::accumulate(optimal.begin(), optimal.end(), 0, [&](int sum, int i){return sum + values[i];});
        ON_LOG(auto optSize = )std::accumulate(optimal.begin(), optimal.end(), 0, [&](int sum, int i){return sum + sizes[i];});
        LOGLN("Opt size " << optSize << " opt " << opt_0_1);
        LOGLN("");

        auto objects = boost::irange(std::size_t(0), values.size());

        //KNAPSACK
        auto opt = detail_knapsack<pd::integral_value_and_size_tag, pd::no_zero_one_tag>(objects, capacity, sizesFunct, valuesFunct).first;

        auto maxValue = detail_knapsack<pd::integral_value_tag, pd::no_zero_one_tag>(objects, capacity, sizesFunct, valuesFunct);
        BOOST_CHECK_EQUAL(opt, maxValue.first);
        maxValue = detail_knapsack<pd::integral_size_tag, pd::no_zero_one_tag>(objects, capacity, sizesFunct, valuesFunct);
        BOOST_CHECK_EQUAL(opt, maxValue.first);

        //KNAPSACK 0/1

        maxValue = detail_knapsack<pd::integral_value_and_size_tag, pd::zero_one_tag>(objects, capacity, sizesFunct, valuesFunct);
        BOOST_CHECK_EQUAL(opt_0_1, maxValue.first);
        maxValue = detail_knapsack<pd::integral_size_tag, pd::zero_one_tag>(objects, capacity, sizesFunct, valuesFunct);
        BOOST_CHECK_EQUAL(opt_0_1, maxValue.first);
        maxValue = detail_knapsack<pd::integral_value_tag, pd::zero_one_tag>(objects, capacity, sizesFunct, valuesFunct);
        BOOST_CHECK_EQUAL(opt_0_1, maxValue.first);

        maxValue = detail_knapsack<pd::integral_value_and_size_tag, pd::zero_one_tag, pd::no_retrieve_solution_tag>(objects, capacity, sizesFunct, valuesFunct);
        BOOST_CHECK_EQUAL(opt_0_1, maxValue.first);
        maxValue = detail_knapsack<pd::integral_size_tag, pd::zero_one_tag, pd::no_retrieve_solution_tag>(objects, capacity, sizesFunct, valuesFunct);
        BOOST_CHECK_EQUAL(opt_0_1, maxValue.first);
        maxValue = detail_knapsack<pd::integral_value_tag, pd::zero_one_tag, pd::no_retrieve_solution_tag>(objects, capacity, sizesFunct, valuesFunct);
        BOOST_CHECK_EQUAL(opt_0_1, maxValue.first);

        //FPTAS
        auto epsilons = {
            0.00001, 0.0001, 0.001, 0.01, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7
            };

        for(auto epsilon : epsilons) {
            //KNAPSACK
            maxValue = detail_knapsack_fptas<pd::no_zero_one_tag, pd::retrieve_solution_tag>(epsilon, objects, capacity, sizesFunct, valuesFunct, on_value_tag());
            BOOST_CHECK(double(opt) * (1. - epsilon) <= maxValue.first);
            BOOST_CHECK(capacity  >= maxValue.second);

            maxValue = detail_knapsack_fptas<pd::no_zero_one_tag, pd::retrieve_solution_tag>(epsilon, objects, capacity, sizesFunct, valuesFunct, on_size_tag());
            BOOST_CHECK(opt <= maxValue.first);
            BOOST_CHECK(double(capacity) * (1. + epsilon) >= maxValue.second);

            //KNAPSACK 0_1
            maxValue = detail_knapsack_fptas<pd::zero_one_tag, pd::retrieve_solution_tag>(epsilon, objects, capacity, sizesFunct, valuesFunct, on_value_tag());
            BOOST_CHECK(double(opt_0_1) * (1. - epsilon) <= maxValue.first);
            BOOST_CHECK(capacity  >= maxValue.second);

            maxValue = detail_knapsack_fptas<pd::zero_one_tag, pd::retrieve_solution_tag>(epsilon, objects, capacity, sizesFunct, valuesFunct, on_size_tag());
            BOOST_CHECK(opt_0_1 <= maxValue.first);
            BOOST_CHECK(double(capacity) * (1. + epsilon) >= maxValue.second);
        }
    });
}
