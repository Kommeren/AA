/**
 * @file knapsack_two_app_long_test.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-09-20
 */

#include <fstream>

#include <boost/test/unit_test.hpp>
#include <boost/range/irange.hpp>

#include "paal/greedy/knapsack_0_1_two_app.hpp"
#include "paal/greedy/knapsack_two_app.hpp"
#include "paal/utils/floating.hpp"

#include "utils/logger.hpp"
#include "utils/read_knapsack.hpp"
#include "utils/parse_file.hpp"

using namespace paal;
using namespace paal::utils;

BOOST_AUTO_TEST_CASE(KnapsackTwoAppLong) {
    std::string testDir = "test/data/KNAPSACK/";

    parse(testDir + "cases.txt", [&](const std::string & line, std::istream &){
        int testId = std::stoi(line);
        LOGLN("test >>>>>>>>>>>>>>>>>>>>>>>>>>>> " << testId);

        int capacity;
        std::vector<int> sizes;
        std::vector<int> values;
        std::vector<int> optimal;

        read(testDir + "cases/", testId, capacity, sizes, values, optimal);
        LOGLN("capacity " << capacity);
        LOGLN("sizes ");
        LOG_COPY_RANGE_DEL(sizes, " ");
        LOGLN("\nvalues ");
        LOG_COPY_RANGE_DEL(values, " ");
        LOGLN("");
        auto objects = boost::irange(std::size_t(0), values.size());
        //KNAPSACK 0/1
        {
            std::vector<int> result;
            LOGLN("Knapsack 0/1");
            auto maxValue = knapsack_0_1_two_app(std::begin(objects), std::end(objects),
                   capacity,
                   std::back_inserter(result),
                   make_ArrayToFunctor(values),
                   make_ArrayToFunctor(sizes));

            LOGLN("Max value " << maxValue.first << ", Total size "  << maxValue.second);
            LOG_COPY_RANGE_DEL(result, " ");
            LOGLN("");
            auto opt = std::accumulate(optimal.begin(), optimal.end(), 0, [&](int sum, int i){return sum + values[i];});
            LOGLN("APPROXIMATION RATIO " << double(opt) /  double(maxValue.first));
            LOGLN("OPT");
            LOG_COPY_RANGE_DEL(optimal, " ");
            LOGLN("");
            BOOST_CHECK(opt <= 2 * maxValue.first);
        }

        //KNAPSACK
        {
            std::vector<int> result;
            LOGLN("Knapsack");
            ON_LOG(auto maxValue = )knapsack_two_app(std::begin(objects), std::end(objects),
                   capacity,
                   std::back_inserter(result),
                   make_ArrayToFunctor(values),
                   make_ArrayToFunctor(sizes));

            LOGLN("Max value " << maxValue.first << ", Total size "  << maxValue.second);
            LOG_COPY_RANGE_DEL(result, " ");
            LOGLN("");
        }
    });
}
