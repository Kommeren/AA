/**
 * @file k_median_example.cpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */
#include <iostream>
#include "paal/local_search/k_median/k_median.hpp"
#include "test/utils/sample_graph.hpp"


int main() {
//! [K Median Search Example]
    // sample data
    typedef SampleGraphsMetrics SGM;
    auto gm = SGM::getGraphMetricSmall();
    
    //define voronoi and solution
    const int k = 2;
    typedef paal::data_structures::Voronoi<decltype(gm)> VorType;
    typedef paal::data_structures::KMedianSolution
        <VorType> Sol;
    typedef paal::data_structures::VoronoiTraits<VorType> VT;
    typedef typename VT::GeneratorsSet GSet;
    typedef typename VT::VerticesSet VSet;
    typedef typename Sol::UnchosenFacilitiesSet USet;


    //create voronoi and solution
    VorType voronoi(GSet{SGM::B, SGM::D}, VSet{SGM::A,SGM::B,SGM::C,SGM::D,SGM::E}, gm);
    Sol sol(std::move(voronoi), USet{SGM::A, SGM::C},k);

    //create facility location local search step
    paal::local_search::k_median::DefaultKMedianComponents<int>::type swap;

    //search 
    paal::local_search::facility_location::facility_location_local_search_simple(sol, swap);

    //print result
    auto const & ch = sol.getChosenFacilities();
    std::cout << "Solution:" << std::endl;
    std::copy(ch.begin(), ch.end(), std::ostream_iterator<int>(std::cout,","));
    std::cout << std::endl;
//! [K Median Search Example]

    return 0;
    
}
