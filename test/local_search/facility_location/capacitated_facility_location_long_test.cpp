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
#include "utils/test_result_check.hpp"

using namespace paal::local_search::facility_location;
using namespace paal;


template <typename Metric, typename Cost>
class fl_logger  {
public:
fl_logger(const Metric & m, const Cost & c) :
            m_metric(m), m_cost(c) {}

template <typename Sol>
bool operator()(Sol & sol) {
           ON_LOG(auto const & ch =  sol.getfacility_location_solution().get_chosen_facilities());
           LOG_COPY_RANGE_DEL(ch, ",");
           ON_LOG(auto c = sol.getfacility_location_solution().get_voronoi().get_cost());
           LOGLN("current cost " << simple_algo::get_cfl_cost(m_metric, m_cost, sol.getfacility_location_solution())
                   << " (dist to full assign " <<  c.get_dist_to_full_assignment()<< ")");
           return true;
        }

private:
    const Metric & m_metric;
    const Cost & m_cost;
};

template <typename Metric, typename Cost>
fl_logger<Metric, Cost>
make_fl_logger(const Metric & m , const Cost & c)  {
    return fl_logger<Metric, Cost>(m, c);
};



template <typename Solve>
void run_tests(const std::string & fname, Solve solve) {
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
        auto metric = paal::read_orlib_FL<cap::capacitated>(ifs, facCost, facCap, demands, fac, clients);
        int firstClient = clients.front();

        auto cost = paal::utils::make_array_to_functor(facCost);
        auto verticesDemands = paal::utils::make_array_to_functor(demands, -firstClient);
        auto facCapacities = paal::utils::make_array_to_functor(facCap);
        LOGLN( "demands sum" << std::accumulate(clients.begin(), clients.end(),
                                    0, [&](int d, int v){return d + verticesDemands(v);}));

        typedef paal::data_structures::capacitated_voronoi<decltype(metric), decltype(facCapacities), decltype(verticesDemands)> VorType;

        typedef paal::data_structures::facility_location_solution
            <decltype(cost), VorType> Sol;
        typedef typename VorType::Generators FSet;
        typedef typename VorType::Vertices VSet;
        typedef typename Sol::UnchosenFacilitiesSet USet;

        VorType voronoi( FSet{fac.begin(), fac.end()},  VSet(clients.begin(), clients.end()), metric, facCapacities, verticesDemands);
        Sol sol(std::move(voronoi), USet{}, cost);



        solve.template operator()<VorType>(sol, metric, cost, opt, make_fl_logger(metric, cost));

        double c = simple_algo::get_cfl_cost(metric, cost, sol);
        check_result(c,opt,6.,paal::utils::less_equal(),0.01);

    }
}

struct solve_add_remove {
    default_remove_fl_components::type rem;
    default_add_fl_components::type    add;
    utils::always_false nop;

    template <typename VorType, typename Cost, typename Solution, typename Action, typename Metric>
    void operator()(Solution & sol, const Metric & metric, Cost cost, double opt, Action a) {
        facility_location_local_search(sol, a, nop, rem);
        facility_location_local_search(sol, a, nop, rem, add);

        ON_LOG(double c = simple_algo::get_cfl_cost(metric, cost, sol));
        LOGLN(std::setprecision(20) << "BEFORE SWAP APPROXIMATION RATIO: " << c / opt);
    }
};

struct solve_add_remove_swap : public solve_add_remove {
    default_swap_fl_components::type   swap;

    template <typename VorType, typename Cost, typename Solution, typename Action, typename Metric>
    void operator()(Solution & sol, const Metric & metric, Cost cost, double opt, Action a) {
        paal::local_search::choose_first_better_strategy strategy;
        facility_location_local_search(sol, strategy, a, nop, rem);
        facility_location_local_search(sol, strategy, a, nop, rem, add);
        facility_location_local_search(sol, strategy, a, nop, rem, add, swap);
    }

};


BOOST_AUTO_TEST_CASE(CapacitatedFacilityLocationLong) {
    run_tests("capopt.txt", solve_add_remove_swap());
}

//currently this is too long !
//hope that in the future stronger algorithms'll handle these cases
//
//
//BOOST_AUTO_TEST_CASE(CapacitatedFacilityLocationVeryLong) {
//    run_tests("capopt_long.txt", solve_add_remove());
//}
