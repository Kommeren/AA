/**
 * @file knapsack_long_test.cpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-09-20
 */

#define BOOST_TEST_MODULE knapsack_long_test

#include <fstream>

#include <boost/test/unit_test.hpp>
#include <boost/range/irange.hpp>

#include "paal/dynamic/knapsack.hpp"
#include "paal/dynamic/knapsack_0_1.hpp"
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
       

        read(testDir + "cases/", testId, capacity, sizes, values, optimal);
        LOGLN("capacity " << capacity);
        LOGLN("sizes ");

        LOG_COPY_DEL(sizes.begin(), sizes.end(), " ");
        LOGLN("values ");
        LOG_COPY_DEL(values.begin(), values.end(), " ");
        auto objects = boost::irange(size_t(0), values.size());
        //KNAPSACK
        {
            std::vector<int> result;
            LOGLN("Knapsack");
            auto maxValue = knapsack(std::begin(objects), std::end(objects), 
                   capacity,
                   std::back_inserter(result), 
                   make_Array2Functor(sizes), 
                   make_Array2Functor(values));

            LOGLN("Max value " << maxValue.first << ", Total size "  << maxValue.second);
            LOG_COPY_DEL(result.begin(), result.end(), " ");
        }
        
        //KNAPSACK 0/1
        {
            std::vector<int> result;
            LOGLN("Knapsack 0/1");
            auto maxValue = knapsack_0_1(std::begin(objects), std::end(objects), 
                   capacity,
                   std::back_inserter(result), 
                   make_Array2Functor(sizes), 
                   make_Array2Functor(values));

            LOGLN("Max value " << maxValue.first << ", Total size "  << maxValue.second);
            LOG_COPY_DEL(result.begin(), result.end(), " ");
            LOGLN("Optimal");
            LOG_COPY_DEL(optimal.begin(), optimal.end(), " ");
            auto opt = std::accumulate(optimal.begin(), optimal.end(), 0, [&](int sum, int i){return sum + values[i];});
            auto optSize = std::accumulate(optimal.begin(), optimal.end(), 0, [&](int sum, int i){return sum + sizes[i];});
            LOGLN("Opt size " << optSize);
            BOOST_CHECK_EQUAL(opt, maxValue.first);
        }

    }
}
