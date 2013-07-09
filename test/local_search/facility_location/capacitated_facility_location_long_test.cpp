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
#include "paal/utils/array2function.hpp"
#include "paal/data_structures/facility_location/fl_algo.hpp"

#include "utils/logger.hpp"
#include "utils/read_orlib_fl.hpp"

using namespace paal::local_search::facility_location;
using namespace paal;

bool le(double x, double y) {
    static const double epsilon = 0.001;
    return x * (1 - epsilon) <= y;
}



template <typename Solve>
void runTests(const std::string & fname, Solve solve) {
    std::string testDir = "test/data/CFL_ORLIB/";
    std::ifstream is_test_cases(testDir + fname);

    assert(is_test_cases.good());
    while(is_test_cases.good()) {
        std::string fname;
        double opt;
        is_test_cases >> fname >> opt;
        opt = cast(opt);
        if(fname == "")
            return;

        LOG("TEST " << fname);
        LOG(std::setprecision(20) <<  "OPT " << opt);

        std::ifstream ifs(testDir + "/cases/" + fname + ".txt");
        assert(ifs.good());
        std::vector<long long> facCost;
        std::vector<int> facCap;
        std::vector<int> demands;
        boost::integer_range<int> fac(0,0);
        boost::integer_range<int> clients(0,0);
        auto metric = paal::readORLIB_FL<cap::capacitated>(ifs, facCost, facCap, demands, fac, clients);
        int firstClient = clients.front();
    
        auto cost = make_Array2Function(facCost);
        auto verticesDemands = make_Array2Function(demands, -firstClient);
        auto facCapacities = make_Array2Function(facCap);
        std::cout << std::accumulate(clients.begin(), clients.end(), 
                                    0, [&](int d, int v){return d + verticesDemands(v);}) << std::endl;
    
        typedef paal::data_structures::CapacitatedVoronoi<decltype(metric), decltype(facCapacities), decltype(verticesDemands)> VorType;

        typedef paal::data_structures::FacilityLocationSolution
            <decltype(cost), VorType> Sol;
        typedef typename VorType::Generators FSet;
        typedef typename VorType::Vertices VSet;
        typedef typename Sol::UnchosenFacilitiesSet USet;

        VorType voronoi( FSet{fac.begin(), fac.end()},  VSet(clients.begin(), clients.end()), metric, facCapacities, verticesDemands);
        Sol sol(std::move(voronoi), USet{}, cost);


        auto logger =  [&](data_structures::ObjectWithCopy<Sol> & sol) {
           LOG_COPY_DEL(sol->getChosenFacilities().begin(), sol->getChosenFacilities().end(), ",");
           ON_LOG(auto c = sol->getVoronoi().getCost());
           LOG("current cost " << simple_algo::getCFLCost(metric, cost, sol.getObj()) << " (dist to full assign " <<  c.getDistToFullAssignment()<< ")");
        };

        auto s = solve.template operator()<VorType>(std::move(sol), metric, cost, opt, logger);

        double c = simple_algo::getCFLCost(metric, cost, s);
        LOG(std::setprecision(20) <<  "cost " << c);
        BOOST_CHECK(le(opt, c));
        LOG(std::setprecision(20) << "APPROXIMATION RATIO: " << c / opt);
    }
}

struct SolveAddRemove {

    template <typename VorType, typename Cost, typename Solution, typename Action, typename Metric>
    Solution operator()(Solution sol, Metric metric, Cost cost, double opt, Action a) {
        
        FacilityLocationLocalSearchStep<VorType, Cost, DefaultRemoveFLComponents<int>::type>  
            lsRemove(std::move(sol), typename DefaultRemoveFLComponents<int>::type());

        search(lsRemove, a);
        
        FacilityLocationLocalSearchStep<VorType, Cost, DefaultRemoveFLComponents<int>::type, DefaultAddFLComponents<int>::type>  
            lsRemoveAdd(std::move(lsRemove.getSolution().getObj()), typename DefaultRemoveFLComponents<int>::type(), typename DefaultAddFLComponents<int>::type());

        search(lsRemoveAdd, a);
        
        auto const & s1 = lsRemoveAdd.getSolution().getObj();
        double c1 = simple_algo::getCFLCost(metric, cost, s1);
        LOG(std::setprecision(20) << "BEFORE SWAP APPROXIMATION RATIO: " << c1 / opt);
        return s1;
    }
};

struct SolveAddRemoveSwap {
    
    template <typename VorType, typename Cost, typename Solution, typename Action, typename Metric>
    Solution operator()(Solution sol, Metric metric, Cost cost, double opt, Action a) {
        auto s = SolveAddRemove().template operator()<VorType>(std::move(sol), metric, cost, opt, a);

        FacilityLocationLocalSearchStep<VorType, decltype(cost), DefaultRemoveFLComponents<int>::type, DefaultAddFLComponents<int>::type, DefaultSwapFLComponents<int>::type>  
            ls(std::move(s), typename DefaultRemoveFLComponents<int>::type(), typename DefaultAddFLComponents<int>::type(), typename DefaultSwapFLComponents<int>::type());

        search(ls, a);
        return ls.getSolution().getObj();
    }
        
};


BOOST_AUTO_TEST_CASE(FacilityLocationLong) {
    runTests("capopt.txt", SolveAddRemoveSwap());
}

BOOST_AUTO_TEST_CASE(FacilityLocationVeryLong) {
    runTests("capopt_long.txt", SolveAddRemove());
}
