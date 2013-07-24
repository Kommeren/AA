/**
 * @file capacitated_facility_location_example.cpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */
#include "paal/local_search/facility_location/facility_location.hpp"
#include "paal/data_structures/voronoi/capacitated_voronoi.hpp"
#include "paal/utils/array2function.hpp"
#include "test/utils/sample_graph.hpp"

using namespace paal::local_search::facility_location;

int main() {
//! [CFL Search Example]
    // sample data
    typedef SampleGraphsMetrics SGM;
    auto gm = SGM::getGraphMetricSmall();
   
    std::vector<int> fcostsv{7,8};
    auto facilityCost = make_Array2Function(fcostsv);

    std::vector<int> fcapv{2, 2};
    auto facilityCapacity = make_Array2Function(fcapv);
    
    std::vector<int> cdemv{2, 2, 1, 3, 3};
    auto clientDemand = make_Array2Function(cdemv);
    
    //define voronoi and solution
    typedef paal::data_structures::CapacitatedVoronoi<
        decltype(gm), decltype(facilityCapacity), decltype(clientDemand)> VorType;
    typedef paal::data_structures::FacilityLocationSolution
        <decltype(facilityCost), VorType> Sol;
    typedef paal::data_structures::VoronoiTraits<VorType> VT;
    typedef typename VT::GeneratorsSet GSet;
    typedef typename VT::VerticesSet VSet;
    typedef typename Sol::UnchosenFacilitiesSet USet;

    //create voronoi and solution
    VorType voronoi(GSet{SGM::A}, VSet{SGM::A,SGM::B,SGM::C,SGM::D,SGM::E}, gm, facilityCapacity, clientDemand);
    Sol sol(std::move(voronoi), USet{SGM::B}, facilityCost);

    //search 
    facility_location_local_search_simple(sol, DefaultRemoveFLComponents<int>::type(), DefaultAddFLComponents<int>::type(), DefaultSwapFLComponents<int>::type());

    //print result
    auto const & ch = sol.getChosenFacilities();
    std::copy(ch.begin(), ch.end(), std::ostream_iterator<int>(std::cout,","));
    std::cout << std::endl;
//! [CFL Search Example]

    return 0;
    
}
