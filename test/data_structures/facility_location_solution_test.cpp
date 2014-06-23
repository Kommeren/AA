/**
 * @file facility_location_solution_test.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-06-20
 */

#include "utils/sample_graph.hpp"

#include "paal/data_structures/facility_location/facility_location_solution.hpp"
#include "paal/data_structures/voronoi/voronoi.hpp"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(facility_location_solutionTest) {
    typedef sample_graphs_metrics SGM;
    auto gm = SGM::get_graph_metric_small();
    std::vector<int> fcosts{ 7, 8 };
    auto cost = [&](int i) { return fcosts[i]; };

    typedef paal::data_structures::voronoi<decltype(gm)> VorType;
    typedef paal::data_structures::facility_location_solution<decltype(cost),
                                                              VorType> Sol;
    typedef typename VorType::GeneratorsSet FSet;
    VorType voronoi(FSet{}, FSet{ SGM::A, SGM::B, SGM::C, SGM::D, SGM::E }, gm);

    Sol sol(voronoi, FSet{ SGM::A, SGM::B }, cost);

    sol.add_facility(SGM::A);
    auto ab_min_a = sol.add_facility(SGM::B);
    auto b_min_ab = sol.rem_facility(SGM::A);
    BOOST_CHECK_EQUAL(sol.add_facility(SGM::A), -b_min_ab);
    BOOST_CHECK_EQUAL(sol.rem_facility(SGM::B), -ab_min_a);
}
