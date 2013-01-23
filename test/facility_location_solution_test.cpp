#define BOOST_TEST_MODULE cycle_manager

#include <boost/test/unit_test.hpp>
#include "data_structures/facility_location_solution.hpp"
#include "utils/sample_graph.hpp"

using namespace paal::data_structures;

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

/*    for(int i : {SGM::A,SGM::B,SGM::C,SGM::D,SGM::E}) {
        std::cout << i << "," << gm(int(SGM::A), i) << std::endl;
    }
    
    for(int i : {SGM::A,SGM::B,SGM::C,SGM::D,SGM::E}) {
        std::cout << i << "," << gm(int(SGM::B), i) << std::endl;
    }*/

    sol.addFacility(SGM::A);
    auto ab_min_a = sol.addFacility(SGM::B);
    auto b_min_ab = sol.remFacility(SGM::A);
    BOOST_CHECK_EQUAL(sol.addFacility(SGM::A), -b_min_ab );
    BOOST_CHECK_EQUAL(sol.remFacility(SGM::B), -ab_min_a);

}
