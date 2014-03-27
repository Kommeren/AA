/**
 * @file facility_location_long_test.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-15
 */

#include <iterator>
#include <iostream>
#include <iomanip>

#include <boost/test/unit_test.hpp>
#include <boost/range/irange.hpp>

#include "paal/local_search/facility_location/facility_location.hpp"
#include "paal/utils/functors.hpp"
#include "paal/data_structures/facility_location/fl_algo.hpp"

#include "utils/logger.hpp"
#include "utils/read_orlib_fl.hpp"
#include "utils/parse_file.hpp"
#include "utils/test_result_check.hpp"

using namespace paal::local_search::facility_location;
using namespace paal;

BOOST_AUTO_TEST_CASE(FacilityLocationLong) {
    std::string testDir = "test/data/FL_ORLIB/";
    parse(testDir + "uncapopt.txt", [&](const std::string & fname, std::istream & is_test_cases) {
        double optTemp;
        is_test_cases >> optTemp;
        long long opt = cast(optTemp);

        LOGLN("TEST " << fname);
        LOGLN(std::setprecision(20) <<  "OPT " << opt);

        std::ifstream ifs(testDir + "/cases/" + fname);
        std::vector<long long> facCost;
        std::vector<int> facCap;
        std::vector<int> demands;
        boost::integer_range<int> fac(0,0);
        boost::integer_range<int> clients(0,0);
        auto metric = paal::read_orlib_FL<cap::uncapacitated>(ifs, facCost, facCap, demands, fac, clients);

        auto cost = paal::utils::make_array_to_functor(facCost);

        typedef paal::data_structures::voronoi<decltype(metric)> VorType;

        typedef paal::data_structures::facility_location_solution
            <decltype(cost), VorType> Sol;
        typedef typename VorType::GeneratorsSet FSet;
        VorType voronoi( FSet{},  FSet(clients.begin(), clients.end()), metric);
        Sol sol(std::move(voronoi), FSet(fac.begin(), fac.end()), cost);

        default_remove_fl_components<int>::type rem;
        default_add_fl_components<int>::type    add;
        default_swap_fl_components<int>::type   swap;
        utils::return_false_functor nop;

        facility_location_local_search(sol, paal::local_search::choose_first_better_strategy{},
                    nop, nop, rem, add, swap);

        long long c = simple_algo::get_fl_cost(metric, cost, sol);
        check_result(c,opt,3.,paal::utils::less_equal(),MULTIPL);
    });

}
