/**
 * @file facility_location_long_test.cpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-15
 */

#include <iterator>
#include <iostream>
#include <fstream>
#include <iomanip>

#include <boost/test/unit_test.hpp>
#include <boost/range/irange.hpp>

#include "paal/local_search/facility_location/facility_location.hpp"
#include "paal/utils/functors.hpp"
#include "paal/utils/floating.hpp"
#include "paal/data_structures/facility_location/fl_algo.hpp"

#include "utils/logger.hpp"
#include "utils/read_orlib_fl.hpp"

using namespace paal::local_search::facility_location;
using namespace paal;

BOOST_AUTO_TEST_CASE(FacilityLocationLong) {
    std::string testDir = "test/data/FL_ORLIB/";
    std::ifstream is_test_cases(testDir + "uncapopt.txt");

    assert(is_test_cases.good());
    while(is_test_cases.good()) {
        std::string fname;
        double opt;
        is_test_cases >> fname >> opt;
        opt = cast(opt);
        if(fname == "")
            return;

        LOGLN("TEST " << fname);
        LOGLN(std::setprecision(20) <<  "OPT " << opt);

        std::ifstream ifs(testDir + "/cases/" + fname);
        std::vector<long long> facCost;
        std::vector<int> facCap;
        std::vector<int> demands;
        boost::integer_range<int> fac(0,0);
        boost::integer_range<int> clients(0,0);
        auto metric = paal::readORLIB_FL<cap::uncapacitated>(ifs, facCost, facCap, demands, fac, clients);
    
        auto cost = paal::utils::make_ArrayToFunctor(facCost);
    
        typedef paal::data_structures::Voronoi<decltype(metric)> VorType;

        typedef paal::data_structures::FacilityLocationSolution
            <decltype(cost), VorType> Sol;
        typedef typename VorType::GeneratorsSet FSet;
        VorType voronoi( FSet{},  FSet(clients.begin(), clients.end()), metric);
        Sol sol(std::move(voronoi), FSet(fac.begin(), fac.end()), cost);
    
        DefaultRemoveFLComponents<int>::type rem;
        DefaultAddFLComponents<int>::type    add;
        DefaultSwapFLComponents<int>::type   swap;
        utils::ReturnFalseFunctor nop;

        facility_location_local_search(sol, nop, nop, rem, add, swap);

        double c = simple_algo::getFLCost(metric, cost, sol);
        LOGLN(std::setprecision(20) <<  "cost " << c);
        BOOST_CHECK(utils::Compare<double>(0.01).le(opt, c));
        LOGLN( std::setprecision(20) << "APPROXIMATION RATIO: " << c / opt);
    }
    
}
