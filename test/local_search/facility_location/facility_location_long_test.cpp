//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file facility_location_long_test.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-15
 */

#include "test_utils/logger.hpp"
#include "test_utils/read_orlib_fl.hpp"
#include "test_utils/test_result_check.hpp"
#include "test_utils/get_test_dir.hpp"
#include "test_utils/system.hpp"

#include "paal/data_structures/facility_location/fl_algo.hpp"
#include "paal/local_search/facility_location/facility_location.hpp"
#include "paal/utils/functors.hpp"
#include "paal/utils/irange.hpp"
#include "paal/utils/parse_file.hpp"

#include <boost/test/unit_test.hpp>

#include <iterator>
#include <iostream>
#include <iomanip>

using namespace paal::local_search;
using namespace paal;

BOOST_AUTO_TEST_CASE(FacilityLocationLong) {
    std::string test_dir = paal::system::get_test_data_dir("FL_ORLIB");
    using paal::system::build_path;
    parse(build_path(test_dir, "uncapopt.txt"),
          [&](const std::string & fname, std::istream & is_test_cases) {
        double optTemp;
        is_test_cases >> optTemp;
        long long opt = cast(optTemp);

        LOGLN("TEST " << fname);
        LOGLN(std::setprecision(20) << "OPT " << opt);

        std::ifstream ifs(build_path(test_dir, "cases/" + fname));
        assert(ifs.good());
        std::vector<long long> facCost;
        std::vector<int> facCap;
        std::vector<int> demands;
        boost::integer_range<int> fac(0, 0);
        boost::integer_range<int> clients(0, 0);
        auto metric = paal::read_orlib_FL<cap::uncapacitated>(
            ifs, facCost, facCap, demands, fac, clients);

        auto cost = paal::utils::make_array_to_functor(facCost);

        typedef paal::data_structures::voronoi<decltype(metric)> VorType;

        typedef paal::data_structures::facility_location_solution<
            decltype(cost), VorType> Sol;
        typedef typename VorType::GeneratorsSet FSet;
        VorType voronoi(FSet{}, FSet(clients.begin(), clients.end()), metric);
        Sol sol(std::move(voronoi), FSet(fac.begin(), fac.end()), cost);

        default_remove_fl_components rem;
        default_add_fl_components add;
        default_swap_fl_components swap;
        utils::always_false nop;

        facility_location_local_search(
            sol, paal::local_search::first_improving_strategy{}, nop, nop, rem,
            add, swap);

        long long c = simple_algo::get_fl_cost(metric, cost, sol);
        check_result(c, opt, 3., paal::utils::less_equal(), MULTIPL);
    });
}
