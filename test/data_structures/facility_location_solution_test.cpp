/**
 * @file facility_location_solution_test.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-06-20
 */

#include <boost/test/unit_test.hpp>
#include "paal/data_structures/facility_location/facility_location_solution.hpp"
#include "paal/data_structures/voronoi/voronoi.hpp"
#include "utils/sample_graph.hpp"

BOOST_AUTO_TEST_CASE(FacilityLocationSolutionTest) {
    typedef SampleGraphsMetrics SGM;
    auto gm = SGM::getGraphMetricSmall();
    std::vector<int> fcosts{7,8};
    auto cost = [&](int i){ return fcosts[i];};

    typedef paal::data_structures::Voronoi<decltype(gm)> VorType;
    typedef paal::data_structures::FacilityLocationSolution
        < decltype(cost), VorType> Sol;
    typedef typename VorType::GeneratorsSet FSet;
    VorType voronoi( FSet{},   FSet{SGM::A,SGM::B,SGM::C,SGM::D,SGM::E}, gm);

    Sol sol(voronoi, FSet{SGM::A, SGM::B}, cost);

    sol.addFacility(SGM::A);
    auto ab_min_a = sol.addFacility(SGM::B);
    auto b_min_ab = sol.remFacility(SGM::A);
    BOOST_CHECK_EQUAL(sol.addFacility(SGM::A), -b_min_ab );
    BOOST_CHECK_EQUAL(sol.remFacility(SGM::B), -ab_min_a);

}
