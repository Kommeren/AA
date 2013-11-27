/**
 * @file k_median_long_test.cpp
 * @brief 
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2013-08-01
 */

#include <iterator>
#include <iostream>
#include <fstream>
#include <iomanip>

#include <boost/test/unit_test.hpp>
#include <boost/range/irange.hpp>

#include "paal/local_search/k_median/k_median.hpp"
#include "paal/utils/functors.hpp"
#include "paal/utils/floating.hpp"
#include "paal/data_structures/facility_location/fl_algo.hpp"

#include "utils/logger.hpp"
#include "utils/read_orlib_km.hpp"
#include "utils/parse_file.hpp"

using namespace paal::local_search::facility_location;
using namespace paal;

BOOST_AUTO_TEST_CASE(KMedianLong) {
    std::string testDir = "test/data/KM_ORLIB/";
    parse(testDir + "capopt.txt", [&](const std::string & fname, std::istream & is_test_cases) {
        double opt;
        is_test_cases >> opt;
        
        LOGLN("TEST " << fname);
        LOGLN(std::setprecision(20) <<  "OPT " << opt);

        std::ifstream ifs(testDir + "/cases/" + fname+".txt");
        boost::integer_range<int> fac(0,0);
        boost::integer_range<int> clients(0,0);
        auto metric = paal::readORLIB_KM(ifs, fac, clients);
    
        typedef paal::data_structures::Voronoi<decltype(metric)> VorType;
        typedef paal::data_structures::KMedianSolution
            <VorType> Sol;
        typedef paal::data_structures::VoronoiTraits<VorType> VT;
        typedef typename VorType::GeneratorsSet GSet;
        typedef typename VT::VerticesSet VSet;
        typedef typename Sol::UnchosenFacilitiesSet USet;
        VorType voronoi( GSet{fac.begin(), fac.end()},  VSet(fac.begin(), clients.end()), metric);
        Sol sol(std::move(voronoi), USet(clients.begin(), clients.end()),fac.size());
        paal::local_search::k_median::DefaultKMedianComponents<int>::type swap;

        facility_location_local_search_simple(sol, swap);

        double c = simple_algo::getKMCost(metric, sol);
        LOGLN("chosen ("<< (sol.getChosenFacilities()).size()<<"):");
        VSet chosen=sol.getChosenFacilities();
        LOG_COPY_RANGE_DEL(chosen," ");
        
        LOGLN(std::setprecision(20) <<  "cost " << c);
        BOOST_CHECK(utils::Compare<double>(0.01).le(opt, c));
        LOGLN( std::setprecision(20) << "APPROXIMATION RATIO: " << c / opt);
    });
}
