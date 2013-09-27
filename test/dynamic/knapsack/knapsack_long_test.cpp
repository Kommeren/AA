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
#include "paal/utils/double_rounding.hpp"
#include "utils/logger.hpp"

using namespace paal;
using namespace paal::utils;

void read(const std::string & testDir, 
          const std::string & filePrefix, 
          int & capacity, 
          std::vector<int> & weights, 
          std::vector<int> & values, 
          std::vector<int> & optimal) {

    //read capacity
    std::string fname = filePrefix + "_c.txt";
    std::ifstream ifs(testDir + fname);
    assert(ifs.good());
    ifs >> capacity;
    ifs.close();
    
    //read weights
    fname = filePrefix + "_w.txt";
    ifs.open(testDir + fname);
    assert(ifs.good());
    while(ifs.good()) {
        std::string s;
        ifs >> s;
        if(s == "")
            break;
        int weight = std::stoi(s);
        assert(weight);
        weights.push_back(weight);
    }
    ifs.close();
    
    //read profits
    fname = filePrefix + "_p.txt";
    ifs.open(testDir + fname);
    assert(ifs.good());
    while(ifs.good()) {
        std::string s;
        ifs >> s;
        if(s == "")
            break;
        int val = std::stoi(s);
        assert(val);
        values.push_back(val);
    }
    assert(values.size() == weights.size());
    ifs.close();
    
    //read profits
    fname = filePrefix + "_s.txt";
    ifs.open(testDir + fname);
    assert(ifs.good());
    int idx(0);
    while(ifs.good()) {
        std::string s;
        ifs >> s;
        if(s == "")
            break;
        bool chosen = std::stoi(s);
        if(chosen) {
            optimal.push_back(idx);
        }
        ++idx;
    }
    ifs.close();
}


BOOST_AUTO_TEST_CASE(ShortestSuperstringLong) {
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
        LOG("test >>>>>>>>>>>>>>>>>>>>>>>>>>>> " << testId);
        
        int capacity;
        std::vector<int> sizes;
        std::vector<int> values;
        std::vector<int> optimal;
       
        read(testDir + "cases/", "p0" + std::to_string(testId), capacity, sizes, values, optimal);
        LOG("capacity " << capacity);
        LOG("sizes ");
        LOG_COPY_DEL(sizes.begin(), sizes.end(), " ");
        LOG("values ");
        LOG_COPY_DEL(values.begin(), values.end(), " ");
        auto objects = boost::irange(size_t(0), values.size());
        //KNAPSACK
        {
            std::vector<int> result;
            LOG("Knapsack");
            auto maxValue = knapsack(std::begin(objects), std::end(objects), 
                   capacity,
                   std::back_inserter(result), 
                   make_Array2Functor(sizes), 
                   make_Array2Functor(values));

            LOG("Max value " << maxValue.first << ", Total size "  << maxValue.second);
            LOG_COPY_DEL(result.begin(), result.end(), " ");
        }
        
        //KNAPSACK 0/1
        {
            std::vector<int> result;
            LOG("Knapsack 0/1");
            auto maxValue = knapsack_0_1(std::begin(objects), std::end(objects), 
                   capacity,
                   std::back_inserter(result), 
                   make_Array2Functor(sizes), 
                   make_Array2Functor(values));

            LOG("Max value " << maxValue.first << ", Total size "  << maxValue.second);
            LOG_COPY_DEL(result.begin(), result.end(), " ");
            LOG("Optimal");
            LOG_COPY_DEL(optimal.begin(), optimal.end(), " ");
            auto opt = std::accumulate(optimal.begin(), optimal.end(), 0, [&](int sum, int i){return sum + values[i];});
            auto optSize = std::accumulate(optimal.begin(), optimal.end(), 0, [&](int sum, int i){return sum + sizes[i];});
            LOG("Opt size " << optSize);
            BOOST_CHECK_EQUAL(opt, maxValue.first);
        }

    }
}
