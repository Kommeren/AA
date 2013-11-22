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

        auto sizesFunct = make_ArrayToFunctor(sizes);
        auto valuesFunct = make_ArrayToFunctor(values);
       

        read(testDir + "cases/", testId, capacity, sizes, values, optimal);
        LOGLN("capacity " << capacity);
        LOGLN("sizes ");

        LOG_COPY_DEL(sizes.begin(), sizes.end(), " ");
        LOGLN("");
        LOGLN("values ");
        LOG_COPY_DEL(values.begin(), values.end(), " ");
        LOGLN("");
        LOGLN("Optimal 0/1");
        LOG_COPY_DEL(optimal.begin(), optimal.end(), " ");
        LOGLN("");
        auto opt_0_1 = std::accumulate(optimal.begin(), optimal.end(), 0, [&](int sum, int i){return sum + values[i];});
        auto optSize = std::accumulate(optimal.begin(), optimal.end(), 0, [&](int sum, int i){return sum + sizes[i];});
        LOGLN("Opt size " << optSize << " opt " << opt_0_1);
        LOGLN("");

        auto objects = boost::irange(size_t(0), values.size());

        //KNAPSACK
        auto opt = detail_knapsack<pd::IntegralValueAndSizeTag, pd::NoZeroOneTag>(objects, capacity, sizesFunct, valuesFunct).first;
       
        auto maxValue = detail_knapsack<pd::IntegralValueTag, pd::NoZeroOneTag>(objects, capacity, sizesFunct, valuesFunct);
        BOOST_CHECK_EQUAL(opt, maxValue.first);
        maxValue = detail_knapsack<pd::IntegralSizeTag, pd::NoZeroOneTag>(objects, capacity, sizesFunct, valuesFunct);
        BOOST_CHECK_EQUAL(opt, maxValue.first);

        //KNAPSACK 0/1
        
        maxValue = detail_knapsack<pd::IntegralValueAndSizeTag, pd::ZeroOneTag>(objects, capacity, sizesFunct, valuesFunct);
        BOOST_CHECK_EQUAL(opt_0_1, maxValue.first);
        maxValue = detail_knapsack<pd::IntegralSizeTag, pd::ZeroOneTag>(objects, capacity, sizesFunct, valuesFunct);
        BOOST_CHECK_EQUAL(opt_0_1, maxValue.first);
        maxValue = detail_knapsack<pd::IntegralValueTag, pd::ZeroOneTag>(objects, capacity, sizesFunct, valuesFunct);
        BOOST_CHECK_EQUAL(opt_0_1, maxValue.first);
        
        maxValue = detail_knapsack<pd::IntegralValueAndSizeTag, pd::ZeroOneTag, pd::NoRetrieveSolutionTag>(objects, capacity, sizesFunct, valuesFunct);
        BOOST_CHECK_EQUAL(opt_0_1, maxValue.first);
        maxValue = detail_knapsack<pd::IntegralSizeTag, pd::ZeroOneTag, pd::NoRetrieveSolutionTag>(objects, capacity, sizesFunct, valuesFunct);
        BOOST_CHECK_EQUAL(opt_0_1, maxValue.first);
        maxValue = detail_knapsack<pd::IntegralValueTag, pd::ZeroOneTag, pd::NoRetrieveSolutionTag>(objects, capacity, sizesFunct, valuesFunct);
        BOOST_CHECK_EQUAL(opt_0_1, maxValue.first);
        
        //FPTAS
        auto epsilons = {
            0.00001, 0.0001, 0.001, 0.01, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7
            };

        for(auto epsilon : epsilons) {
            //KNAPSACK
            maxValue = detail_knapsack_fptas<pd::NoZeroOneTag, pd::RetrieveSolutionTag>(epsilon, objects, capacity, sizesFunct, valuesFunct, OnValueTag());
            BOOST_CHECK(double(opt) * (1. - epsilon) <= maxValue.first);
            BOOST_CHECK(capacity  >= maxValue.second);
            
            maxValue = detail_knapsack_fptas<pd::NoZeroOneTag, pd::RetrieveSolutionTag>(epsilon, objects, capacity, sizesFunct, valuesFunct, OnSizeTag());
            BOOST_CHECK(opt <= maxValue.first);
            BOOST_CHECK(double(capacity) * (1. + epsilon) >= maxValue.second);

            //KNAPSACK 0_1
            maxValue = detail_knapsack_fptas<pd::ZeroOneTag, pd::RetrieveSolutionTag>(epsilon, objects, capacity, sizesFunct, valuesFunct, OnValueTag());
            BOOST_CHECK(double(opt_0_1) * (1. - epsilon) <= maxValue.first);
            BOOST_CHECK(capacity  >= maxValue.second);
            
            maxValue = detail_knapsack_fptas<pd::ZeroOneTag, pd::RetrieveSolutionTag>(epsilon, objects, capacity, sizesFunct, valuesFunct, OnSizeTag());
            BOOST_CHECK(opt_0_1 <= maxValue.first);
            BOOST_CHECK(double(capacity) * (1. + epsilon) >= maxValue.second);
        }
        

    }
}
