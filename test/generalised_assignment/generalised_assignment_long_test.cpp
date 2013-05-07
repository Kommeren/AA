/**
 * @file generalised_assignemnt_long_test.cpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-15
 */

#define BOOST_TEST_MODULE generalised_assignemnt_long

#include <iterator>
#include <iostream>
#include <fstream>
#include <iomanip>

#include <boost/test/unit_test.hpp>
#include <boost/range/irange.hpp>

#include "paal/iterative_rounding/generalised_assignment.hpp"
#include "utils/logger.hpp"
#include "utils/read_gen_ass.hpp"

using namespace paal::ir;
using namespace paal;

bool le(double x, double y) {
    static const double epsilon = 0.01;
    return x * (1 - epsilon) <= y;
}

BOOST_AUTO_TEST_CASE(FacilityLocationLong) {
    std::string testDir = "test/data/GENERALISED_ASSIGNMENT/";
    std::ifstream is_test_cases(testDir + "gapopt.txt");

    assert(is_test_cases.good());
    while(is_test_cases.good()) {
        std::string fname;
        double opt;
        int numberOfCases;
        int MAX_LINE = 256;
        char buf[MAX_LINE];
        is_test_cases.getline(buf, MAX_LINE);
        if(buf[0] == 0) {
            return;
        }
        
        if(buf[0] == '#')
            continue;
        std::stringstream ss;
        ss << buf;
        
        ss >> fname;
        ss >> numberOfCases;

        LOG(fname << " " << numberOfCases);
        for(int i : boost::irange(0, numberOfCases)) {
            is_test_cases >> opt;
            LOG("case " << i << " opt "<< opt);
        

/*        std::ifstream ifs(testDir + "/cases/" + fname);
        std::vector<long long> facCost;
        std::vector<int> facCap;
        std::vector<int> demands;
        boost::integer_range<int> fac(0,0);
        boost::integer_range<int> clients(0,0);
        auto metric = paal::readORLIB_FL<cap::uncapacitated>(ifs, facCost, facCap, demands, fac, clients);
    
        auto cost = [&](int i){ return facCost[i];};
    
        typedef paal::data_structures::Voronoi<decltype(metric)> VorType;

        typedef paal::data_structures::FacilityLocationSolution
            <decltype(cost), VorType> Sol;
        typedef typename VorType::GeneratorsSet FSet;
        VorType voronoi( FSet{},  FSet(clients.begin(), clients.end()), metric);
        Sol sol(std::move(voronoi), FSet(fac.begin(), fac.end()), cost);

        FacilityLocationLocalSearchStep<VorType, decltype(cost)>  
            ls(std::move(sol));

        ON_LOG(auto & s = ls.getSolution().getObj());

        search(ls);
        double c = simple_algo::getFLCost(metric, cost, s);
        LOG(std::setprecision(20) <<  "cost " << c);
        BOOST_CHECK(le(opt, c));
        LOG( std::setprecision(20) << "APPROXIMATION RATIO: " << c / opt);*/
        }
        is_test_cases.getline(buf, MAX_LINE);
    }
    
}
