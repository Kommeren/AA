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

#include <boost/test/unit_test.hpp>
#include "local_search/facility_location/facility_location.hpp"
#include "utils/sample_graph.hpp"
#include "utils/logger.hpp"
#include "utils/read_orlib_fc.hpp"

using namespace paal::local_search::facility_location;

BOOST_AUTO_TEST_CASE(FacilityLocationLong) {

    std::ifstream ifs("test/FL_ORLIB/cases/cap71.txt");
    std::vector<float> facCost;
    std::vector<int> facCap;
    std::vector<int> demands;
    boost::integer_range<int> fac(0,0);
    boost::integer_range<int> clients(0,0);
    auto metric = paal::readORLIB_FC(ifs, facCost, facCap, demands, fac, clients);

    
    auto cost = [&](int i){ return facCost[i];};
    
    typedef paal::data_structures::Voronoi<decltype(metric)> VorType;

    typedef paal::data_structures::FacilityLocationSolution
        <decltype(cost), VorType> Sol;
    typedef typename VorType::GeneratorsSet FSet;
    VorType voronoi( FSet{},  FSet(clients.begin(), clients.end()), metric);

    FacilityLocationLocalSearchStep<VorType, decltype(cost)>  
        ls(voronoi, cost, FSet(fac.begin(), fac.end()));

    while(ls.search()) {
#ifdef LOGGER_ON 
        auto & s = ls.getSolution();
        auto const & ch = s->getChosenFacilities();
        std::copy(ch.begin(), ch.end(), std::ostream_iterator<int>(std::cout,","));
        LOG("");
#endif
    }
    
}
