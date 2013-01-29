#define BOOST_TEST_MODULE facility_location_local_search

#include <iterator>

#include <boost/test/unit_test.hpp>
#include "local_search/facility_location/facility_location.hpp"
#include "utils/sample_graph.hpp"

using namespace paal::local_search::facility_location;

BOOST_AUTO_TEST_CASE(FacilityLocationSolutionTest) {
    
    typedef SampleGraphsMetrics SGM;
    auto gm = SGM::getGraph();
    std::vector<int> fcosts{7,8};
    auto cost = [&](int i){ return fcosts[i];};

    typedef FacilityLocationLocalSearchStep<int, decltype(gm), decltype(cost)> FLLS;

    FLLS  ls(FLLS::FacilitiesSet{}, FLLS::FacilitiesSet{0,1}, FLLS::ClientsSet{0,1,2,3,4}, cost, gm);
    BOOST_CHECK(ls.search());
    auto & s = ls.getSolution();
    auto & ch = s.getChosenFacilities();
    
    std::copy(ch.begin(), ch.end(), std::ostream_iterator<int>(std::cout,","));
    std::cout << std::endl;
    BOOST_CHECK(!ls.search());
    
}
