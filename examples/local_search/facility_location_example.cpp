/**
 * @file facility_location_example.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */
#include <iostream>

#include "paal/local_search/facility_location/facility_location.hpp"
#include "paal/utils/functors.hpp"
#include "test/utils/sample_graph.hpp"

using namespace paal::local_search::facility_location;



int main() {
//! [FL Search Example]
    // sample data
    typedef SampleGraphsMetrics SGM;
    auto gm = SGM::getGraphMetricSmall();
    std::vector<int> fcosts{7,8};
    typedef paal::utils::ArrayToFunctor<std::vector<int>> Cost;

    //define voronoi and solution
    typedef paal::data_structures::Voronoi<decltype(gm)> VorType;
    typedef paal::data_structures::FacilityLocationSolution
        <Cost, VorType> Sol;
    typedef paal::data_structures::VoronoiTraits<VorType> VT;
    typedef typename VT::GeneratorsSet GSet;
    typedef typename VT::VerticesSet VSet;
    typedef typename Sol::UnchosenFacilitiesSet USet;


    //create voronoi and solution
    VorType voronoi(GSet{}, VSet{SGM::A,SGM::B,SGM::C,SGM::D,SGM::E}, gm);
    Sol sol(std::move(voronoi), USet{SGM::A, SGM::B}, paal::utils::make_ArrayToFunctor(fcosts));

    //create facility location local search components
    DefaultRemoveFLComponents<int>::type rem;
    DefaultAddFLComponents<int>::type    add;
    DefaultSwapFLComponents<int>::type   swap;

    //search
    facility_location_local_search_simple(sol, rem, add, swap);

    //print result
    auto const & ch = sol.getChosenFacilities();
    std::copy(ch.begin(), ch.end(), std::ostream_iterator<int>(std::cout,","));
    std::cout << std::endl;
//! [FL Search Example]

    return true;

}
