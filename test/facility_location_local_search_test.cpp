#define BOOST_TEST_MODULE facility_location_local_search

#include <boost/test/unit_test.hpp>
#include "local_search/facility_location/facility_location.hpp"
#include "utils/sample_graph.hpp"

using namespace paal::data_structures;

BOOST_AUTO_TEST_CASE(FacilityLocationSolutionTest) {
    typedef FacilityLocationSolution<int> FCS;
    FCS fls(FCS::FacilitiesSet{1,2,3,4,5});
}
