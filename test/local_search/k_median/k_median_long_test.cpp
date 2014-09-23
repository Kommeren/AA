/**
 * @file k_median_long_test.cpp
 * @brief
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2013-08-01
 */

#include "utils/logger.hpp"
#include "utils/read_orlib_km.hpp"
#include "utils/parse_file.hpp"
#include "utils/test_result_check.hpp"

#include "paal/local_search/k_median/k_median.hpp"
#include "paal/utils/functors.hpp"
#include "paal/utils/floating.hpp"
#include "paal/data_structures/facility_location/fl_algo.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/range/irange.hpp>

#include <iterator>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace paal::local_search;
using namespace paal;

BOOST_AUTO_TEST_CASE(KMedianLong) {
    std::string testDir = "test/data/KM_ORLIB/";
    parse(testDir + "capopt.txt",
          [&](const std::string & fname, std::istream & is_test_cases) {
        double opt;
        is_test_cases >> opt;

        LOGLN("TEST " << fname);
        LOGLN(std::setprecision(20) << "OPT " << opt);

        std::ifstream ifs(testDir + "/cases/" + fname + ".txt");
        boost::integer_range<int> fac(0, 0);
        boost::integer_range<int> clients(0, 0);
        auto metric = paal::read_orlib_KM(ifs, fac, clients);

        typedef paal::data_structures::voronoi<decltype(metric)> VorType;
        typedef paal::data_structures::k_median_solution<VorType> Sol;
        typedef paal::data_structures::voronoi_traits<VorType> VT;
        typedef typename VorType::GeneratorsSet GSet;
        typedef typename VT::VerticesSet VSet;
        typedef typename Sol::UnchosenFacilitiesSet USet;
        VorType voronoi(GSet{ fac.begin(), fac.end() },
                        VSet(fac.begin(), clients.end()), metric);
        Sol sol(std::move(voronoi), USet(clients.begin(), clients.end()),
                fac.size());
        paal::local_search::default_k_median_components swap;

        facility_location_first_improving(sol, swap);

        double c = simple_algo::get_km_cost(metric, sol);
        LOGLN("chosen (" << (sol.get_chosen_facilities()).size() << "):");
        VSet chosen = sol.get_chosen_facilities();
        LOG_COPY_RANGE_DEL(chosen, " ");
        check_result(c, opt, 5., paal::utils::less_equal(), 0.01);
    });
}
