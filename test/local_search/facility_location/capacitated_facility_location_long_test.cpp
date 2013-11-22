/**
 * @file capacitated_facility_location_long_test.cpp
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
#include "paal/data_structures/voronoi/capacitated_voronoi.hpp"
#include "paal/utils/functors.hpp"
#include "paal/utils/floating.hpp"
#include "paal/data_structures/facility_location/fl_algo.hpp"

#include "utils/logger.hpp"
#include "utils/read_orlib_fl.hpp"

using namespace paal::local_search::facility_location;
using namespace paal;


template <typename Metric, typename Cost>
class FLLogger  {
public:
FLLogger(const Metric & m, const Cost & c) :
            m_metric(m), m_cost(c) {}

template <typename Sol>
void operator()(Sol & sol) {
           ON_LOG(auto const & ch =  sol.get().getChosenFacilities());
           LOG_COPY_DEL(ch.begin(), ch.end(), ",");
           ON_LOG(auto c = sol.get().getVoronoi().getCost());
           LOGLN("current cost " << simple_algo::getCFLCost(m_metric, m_cost, sol.get()) << " (dist to full assign " <<  c.getDistToFullAssignment()<< ")");
        };

private:
    const Metric & m_metric;
    const Cost & m_cost;
};

template <typename Metric, typename Cost>
FLLogger<Metric, Cost> 
make_fLLogger(const Metric & m , const Cost & c)  {
    return FLLogger<Metric, Cost>(m, c);
};



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

        LOGLN("TEST " << fname);
        LOGLN(std::setprecision(20) <<  "OPT " << opt);

        std::ifstream ifs(testDir + "/cases/" + fname + ".txt");
        assert(ifs.good());
        std::vector<long long> facCost;
        std::vector<int> facCap;
        std::vector<int> demands;
        boost::integer_range<int> fac(0,0);
        boost::integer_range<int> clients(0,0);
        auto metric = paal::readORLIB_FL<cap::capacitated>(ifs, facCost, facCap, demands, fac, clients);
        int firstClient = clients.front();
    
        auto cost = paal::utils::make_ArrayToFunctor(facCost);
        auto verticesDemands = paal::utils::make_ArrayToFunctor(demands, -firstClient);
        auto facCapacities = paal::utils::make_ArrayToFunctor(facCap);
        LOGLN( "demands sum" << std::accumulate(clients.begin(), clients.end(), 
                                    0, [&](int d, int v){return d + verticesDemands(v);}));
    
        typedef paal::data_structures::CapacitatedVoronoi<decltype(metric), decltype(facCapacities), decltype(verticesDemands)> VorType;

        typedef paal::data_structures::FacilityLocationSolution
            <decltype(cost), VorType> Sol;
        typedef typename VorType::Generators FSet;
        typedef typename VorType::Vertices VSet;
        typedef typename Sol::UnchosenFacilitiesSet USet;

        VorType voronoi( FSet{fac.begin(), fac.end()},  VSet(clients.begin(), clients.end()), metric, facCapacities, verticesDemands);
        Sol sol(std::move(voronoi), USet{}, cost);



        solve.template operator()<VorType>(sol, metric, cost, opt, make_fLLogger(metric, cost));

        double c = simple_algo::getCFLCost(metric, cost, sol);
        LOGLN(std::setprecision(20) <<  "cost " << c);
        BOOST_CHECK(utils::Compare<double>(0.01).le(opt, c));
        LOGLN(std::setprecision(20) << "APPROXIMATION RATIO: " << c / opt);
    }
}

struct SolveAddRemove {
    DefaultRemoveFLComponents<int>::type rem;
    DefaultAddFLComponents<int>::type    add;
    utils::ReturnFalseFunctor nop;

    template <typename VorType, typename Cost, typename Solution, typename Action, typename Metric>
    void operator()(Solution & sol, const Metric & metric, Cost cost, double opt, Action a) {
        facility_location_local_search(sol, a, nop, rem);
        facility_location_local_search(sol, a, nop, rem, add);
        
        ON_LOG(double c = simple_algo::getCFLCost(metric, cost, sol));
        LOGLN(std::setprecision(20) << "BEFORE SWAP APPROXIMATION RATIO: " << c / opt);
    }
};

struct SolveAddRemoveSwap : public SolveAddRemove {
    DefaultSwapFLComponents<int>::type   swap;
    
    template <typename VorType, typename Cost, typename Solution, typename Action, typename Metric>
    void operator()(Solution & sol, const Metric & metric, Cost cost, double opt, Action a) {
        facility_location_local_search(sol, a, nop, rem);
        facility_location_local_search(sol, a, nop, rem, add);
        facility_location_local_search(sol, a, nop, rem, add, swap);
    }
        
};


BOOST_AUTO_TEST_CASE(CapacitatedFacilityLocationLong) {
    runTests("capopt.txt", SolveAddRemoveSwap());
}

//currently this is too long !
//hope that in the future stronger algorithms'll handle these cases
//
//
//BOOST_AUTO_TEST_CASE(CapacitatedFacilityLocationVeryLong) {
//    runTests("capopt_long.txt", SolveAddRemove());
//}
