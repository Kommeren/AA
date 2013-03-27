/**
 * @file capacitated_facility_location_long_test.cpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-15
 */

#define BOOST_TEST_MODULE capacitated_facility_location_local_search_long

#include <iterator>
#include <iostream>
#include <fstream>
#include <iomanip>

#include <boost/test/unit_test.hpp>
#include <boost/range/irange.hpp>

#include "paal/local_search/facility_location/facility_location.hpp"
#include "paal/data_structures/voronoi/capacitated_voronoi.hpp"
#include "utils/logger.hpp"
#include "utils/read_orlib_fl.hpp"
#include "paal/data_structures/facility_location/fl_algo.hpp"

using namespace paal::local_search::facility_location;
using namespace paal;

bool le(double x, double y) {
    static const double epsilon = 0.01;
    return x - epsilon <= y;
}

BOOST_AUTO_TEST_CASE(FacilityLocationLong) {
    std::string testDir = "test/data/CFL_ORLIB/";
    std::ifstream is_test_cases(testDir + "capopt.txt");

    while(is_test_cases.good()) {
        std::string fname;
        double opt;
        is_test_cases >> fname >> opt;
        opt *= MULTIPL;
        if(fname == "")
            return;

        LOG("TEST " << fname);
        LOG(std::setprecision(20) <<  "OPT " << opt);

        std::ifstream ifs(testDir + "/cases/" + fname + ".txt");
        std::vector<long long> facCost;
        std::vector<int> facCap;
        std::vector<int> demands;
        boost::integer_range<int> fac(0,0);
        boost::integer_range<int> clients(0,0);
        auto metric = paal::readORLIB_FL(ifs, facCost, facCap, demands, fac, clients);
        int firstClient = clients.front();
    
        auto cost = [&](int i){ return facCost[i];};
        auto verticesDemands = [&](int i){return demands[i - firstClient]; };
        auto facCapacities = [&](int i){return facCap[i]; };
        std::cout << std::accumulate(clients.begin(), clients.end(), 
                                    0, [&](int d, int v){return d + verticesDemands(v);}) << std::endl;
    
        typedef paal::data_structures::CapacitatedVoronoi<decltype(metric), decltype(facCapacities), decltype(verticesDemands)> VorType;

        typedef paal::data_structures::FacilityLocationSolution
            <decltype(cost), VorType> Sol;
        typedef typename VorType::Generators FSet;
        typedef typename VorType::Vertices VSet;
        typedef typename Sol::UnchosenFacilitiesSet USet;

        VorType voronoi( FSet{},  VSet(clients.begin(), clients.end()), metric, facCapacities, verticesDemands);
        Sol sol(std::move(voronoi), USet(fac.begin(), fac.end()), cost);

        FacilityLocationLocalSearchStep<VorType, decltype(cost)>  
            ls(std::move(sol));

        ON_LOG(auto & s = ls.getSolution().getObj());

        search(ls, [&](data_structures::ObjectWithCopy<Sol> & s) {
           LOG_COPY_DEL(s->getChosenFacilities().begin(), s->getChosenFacilities().end(), ",");
           auto cost = s->getVoronoi().getCost();
           LOG("current cost "<< cost.getDistToFullAssignment() << " " << cost.getRealDist());
        });
        double c = simple_algo::getFLCost(metric, cost, s);
        LOG(std::setprecision(20) <<  "cost " << c);
        BOOST_CHECK(le(opt, c));
        LOG("APPROXIMATION RATIO: " << c / opt);
    }
}
