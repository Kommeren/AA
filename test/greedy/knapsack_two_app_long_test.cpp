/**
 * @file knapsack_two_app_long_test.cpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-09-20
 */

#define BOOST_TEST_MODULE knapsack_long_test

#include <fstream>

#include <boost/test/unit_test.hpp>
#include <boost/range/irange.hpp>

#include "paal/greedy/knapsack_0_1_two_app.hpp"
#include "paal/greedy/knapsack_two_app.hpp"
#include "paal/utils/double_rounding.hpp"
#include "utils/logger.hpp"
#include "utils/read_knapsack.hpp"

using namespace paal;
using namespace paal::utils;

BOOST_AUTO_TEST_CASE(KnapSackLong) {
    std::string testDir = "test/data/KNAPSACK/";
    std::ifstream is_test_cases(testDir + "cases.txt");

    int testId;
    assert(is_test_cases.good());
    while(is_test_cases.good()) {
        std::string line;
        is_test_cases >> line;
        if(line == "")
            return;
        testId = std::stoi(line);
        LOGLN("test >>>>>>>>>>>>>>>>>>>>>>>>>>>> " << testId);
        
        int capacity;
        std::vector<int> sizes;
        std::vector<int> values;
        std::vector<int> optimal;
       
        read(testDir + "cases/",testId, capacity, sizes, values, optimal);
        LOGLN("capacity " << capacity);
        LOGLN("sizes ");
        LOG_COPY_DEL(sizes.begin(), sizes.end(), " ");
        LOGLN("values ");
        LOG_COPY_DEL(values.begin(), values.end(), " ");
        auto objects = boost::irange(size_t(0), values.size());
        //KNAPSACK 0/1
        {
            std::vector<int> result;
            LOGLN("Knapsack 0/1");
            auto maxValue = knapsack_0_1_two_app(std::begin(objects), std::end(objects), 
                   capacity,
                   std::back_inserter(result), 
                   make_ArrayToFunctor(sizes), 
                   make_ArrayToFunctor(values));

            LOGLN("Max value " << maxValue.first << ", Total size "  << maxValue.second);
            LOG_COPY_DEL(result.begin(), result.end(), " ");
            auto opt = std::accumulate(optimal.begin(), optimal.end(), 0, [&](int sum, int i){return sum + values[i];});
            LOG("APPROXIMATION RATIO " << double(opt) /  double(maxValue.first));
            LOG("OPT");
            LOG_COPY_DEL(optimal.begin(), optimal.end(), " ");
            BOOST_CHECK(opt <= 2 * maxValue.first);
        }
        
        //KNAPSACK 
        {
            std::vector<int> result;
            LOGLN("Knapsack");
            auto maxValue = knapsack_two_app(std::begin(objects), std::end(objects), 
                   capacity,
                   std::back_inserter(result), 
                   make_ArrayToFunctor(sizes), 
                   make_ArrayToFunctor(values));

            LOGLN("Max value " << maxValue.first << ", Total size "  << maxValue.second);
            LOG_COPY_DEL(result.begin(), result.end(), " ");
        }
    }
}
