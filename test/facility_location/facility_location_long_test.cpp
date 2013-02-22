/**
 * @file facility_location_long_test.cpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-15
 */

#define BOOST_TEST_MODULE facility_location_local_search_long

#include <iterator>
#include <iostream>
#include <fstream>
#include <iomanip>

#include <boost/test/unit_test.hpp>
#include <boost/range/irange.hpp>

#include "paal/local_search/facility_location/facility_location.hpp"
#include "utils/logger.hpp"
#include "utils/read_orlib_fl.hpp"
#include "paal/simple_algo/fl_algo.hpp"

using namespace paal::local_search::facility_location;
using namespace paal;

bool le(double x, double y) {
    static const double epsilon = 0.01;
    return x - epsilon <= y;
}

BOOST_AUTO_TEST_CASE(FacilityLocationLong) {
    std::string testDir = "test/data/FL_ORLIB/";
    std::ifstream is_test_cases(testDir + "uncapopt.txt");

    while(is_test_cases.good()) {
        std::string fname;
        double opt;
        is_test_cases >> fname >> opt;
        if(fname == "")
            return;

        LOG("TEST " << fname);
        LOG(std::setprecision(20) <<  "OPT " << opt);

        std::ifstream ifs(testDir + "/cases/" + fname);
        std::vector<double> facCost;
        std::vector<int> facCap;
        std::vector<int> demands;
        boost::integer_range<int> fac(0,0);
        boost::integer_range<int> clients(0,0);
        auto metric = paal::readORLIB_FL(ifs, facCost, facCap, demands, fac, clients);

    
        auto cost = [&](int i){ return facCost[i];};
    
        typedef paal::data_structures::Voronoi<decltype(metric)> VorType;

        typedef paal::data_structures::FacilityLocationSolution
            <decltype(cost), VorType> Sol;
        typedef typename VorType::GeneratorsSet FSet;
        VorType voronoi( FSet{},  FSet(clients.begin(), clients.end()), metric);

        FacilityLocationLocalSearchStep<VorType, decltype(cost)>  
            ls(voronoi, cost, FSet(fac.begin(), fac.end()));

#ifdef LOGGER_ON
        auto & s = ls.getSolution().getObj();
//        auto const & ch = s.getChosenFacilities();
#endif
        while(ls.search()) {
//            LOG_COPY_DEL(ch.begin(), ch.end(), ",");
//            LOG(std::setprecision(20) <<  "cost " << simple_algo::getFLCost(metric, cost, s));
        }
        double c = simple_algo::getFLCost(metric, cost, s);
        LOG(std::setprecision(20) <<  "cost " << c);
        BOOST_CHECK(le(opt, c));
        BOOST_CHECK(le(c, (11./6.) * opt));
        LOG("APPROXIMATION RATIO: " << c / opt);
    }
    
}
