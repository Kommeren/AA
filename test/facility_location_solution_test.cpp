#define BOOST_TEST_MODULE cycle_manager

#include <boost/test/unit_test.hpp>
#include "local_search/facility_location/facility_location_solution.hpp"
#include "utils/sample_graph.hpp"

using namespace paal::local_search::facility_location;

BOOST_AUTO_TEST_CASE(FacilityLocationSolutionTest) {
    typedef FacilityLocationSolution<int> FCS;
    FCS fls(FCS::FacilitiesSet{1,2,3,4,5});
    
    fls.add(1);
    fls.add(2);
    fls.add(3);
    fls.remove(2);
    fls.remove(1);
    fls.add(2);
    auto ch = fls.getChosenFacilities();
    auto uch = fls.getUnchosenFacilities();
    BOOST_CHECK(ch.find(3) != ch.end());
    BOOST_CHECK(ch.find(2) != ch.end());
    BOOST_CHECK(ch.find(1) == ch.end());
    BOOST_CHECK(ch.find(4) == ch.end());
    BOOST_CHECK(ch.find(5) == ch.end());

    BOOST_CHECK(uch.find(3) == uch.end());
    BOOST_CHECK(uch.find(2) == uch.end());
    BOOST_CHECK(uch.find(1) != uch.end());
    BOOST_CHECK(uch.find(4) != uch.end());
    BOOST_CHECK(uch.find(5) != uch.end());
}


BOOST_AUTO_TEST_CASE(FacilityLocationSolutionWithClientAssignmentsTest) {
    typedef SampleGraphsMetrics SGM;
    auto gm = SGM::getGraph();
    std::vector<int> fcosts{7,8};
    auto cost = [&](int i){ return fcosts[i];};

    typedef FacilityLocationSolutionWithClientsAssignment
        <int, decltype(gm), decltype(cost)> Sol;
    typedef typename Sol::FacilitiesSet FSet;
    Sol sol(FSet{SGM::A,SGM::B}, FSet{},
            FSet{SGM::A,SGM::B,SGM::C,SGM::D,SGM::E}, gm, cost);

    std::cout << "A " << sol.addFacility(SGM::A) << std::endl;
    std::cout << "A + B "<< sol.addFacility(SGM::B) << std::endl;
    std::cout << "B "<< sol.remFacility(SGM::A) << std::endl;
    std::cout << "A + B "<< sol.addFacility(SGM::A) << std::endl;
    std::cout << "B "<< sol.remFacility(SGM::B) << std::endl;

}
