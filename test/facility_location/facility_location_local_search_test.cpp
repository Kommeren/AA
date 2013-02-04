#define BOOST_TEST_MODULE facility_location_local_search

#include <iterator>

#include <boost/test/unit_test.hpp>
#include "local_search/facility_location/facility_location.hpp"
#include "utils/sample_graph.hpp"

using namespace paal::local_search::facility_location;

BOOST_AUTO_TEST_CASE(FacilityLocationSolutionTest) {
    
    typedef SampleGraphsMetrics SGM;
    auto gm = SGM::getGraphMetric();
    std::vector<int> fcosts{7,8};
    auto cost = [&](int i){ return fcosts[i];};
    
    typedef paal::data_structures::FacilityLocationSolutionWithClientsAssignment
        <int, decltype(gm), decltype(cost)> FLS;

    FLS fls(FLS::FacilitiesSet{0,1}, FLS::FacilitiesSet{}, 
                FLS::ClientsSet{0,1,2,3,4}, gm, cost);

    FacilityLocationLocalSearchStep<FLS>  ls(fls);
    BOOST_CHECK(ls.search());
    auto & s = ls.getSolution();
    auto const & ch = s->getChosenFacilities();
    
    std::copy(ch.begin(), ch.end(), std::ostream_iterator<int>(std::cout,","));
    std::cout << std::endl;
    BOOST_CHECK(!ls.search());
    
}
