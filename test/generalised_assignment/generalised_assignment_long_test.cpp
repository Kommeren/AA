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

#include "paal/iterative_rounding/generalised_assignment/generalised_assignment.hpp"
#include "paal/data_structures/metric/basic_metrics.hpp"
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
        std::ifstream ifs(testDir + "/cases/" + fname + ".txt");
        int num;
        ifs >> num;
        assert(num == numberOfCases);
        for(int i : boost::irange(0, numberOfCases)) {
            is_test_cases >> opt;
            LOG("case " << i << " opt "<< opt);
        

            paal::M costs;
            paal::M times;
            std::vector<int> machinesBounds;
            boost::integer_range<int> machines(0,0);
            boost::integer_range<int> jobs(0,0);
            paal::readGEN_ASS(ifs, costs, times, machinesBounds, machines, jobs);
            auto  Tf = [&](int i){return machinesBounds[i];}; 
            auto ga = make_GeneralAssignement(machines.begin(), machines.end(),
                    jobs.begin(), jobs.end(), 
                    costs, times, Tf);
            times(0, 0);
            IterativeRounding<decltype(ga)> ir(std::move(ga));
            paal::ir::solve(ir);
    
    

            ON_LOG(auto & s = ir.getEngine().getSolution());
            int c(0);
            std::vector<int> machinesLoad(machines.size(), 0);
            for(const std::pair<int, int> & jm : s ) {
        //        LOG("job " << jm.first << " assigned to machine " << jm.second);
                c += costs(jm.first, jm.second);
                machinesLoad[jm.second] += times(jm.first, jm.second);
            }
            double approximationRatio = 1.;
            for(int m : machines) {
                BOOST_CHECK(machinesLoad[m] <= 2 * machinesBounds[m]);
                approximationRatio = std::max(approximationRatio, double(machinesLoad[m]) / double(machinesBounds[m]));
            }

            for(int j : jobs) {
                BOOST_CHECK(s.find(j) != s.end());
            }

            LOG("cost " << c);
            BOOST_CHECK(le(c, opt));
            LOG( std::setprecision(10) << "APPROXIMATION RATIO: " << approximationRatio << " cost / opt = " << double(c) / double(opt));
        }
        is_test_cases.getline(buf, MAX_LINE);
    }
    
}
