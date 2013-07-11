#include <iostream>

#include "paal/local_search/facility_location/facility_location.hpp"
#include "paal/utils/array2function.hpp"
#include "utils/sample_graph.hpp"

using namespace paal::local_search::facility_location;



int main() {
//! [FL Search Example]
    // sample data
    typedef SampleGraphsMetrics SGM;
    auto gm = SGM::getGraphMetricSmall();
    std::vector<int> fcosts{7,8};
    typedef Array2Function<std::vector<int>> Cost;  

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
    Sol sol(std::move(voronoi), USet{SGM::A, SGM::B}, make_Array2Function(fcosts));

    //create facility location local search step
    FacilityLocationLocalSearchStep<VorType, Cost, DefaultRemoveFLComponents<int>::type, DefaultAddFLComponents<int>::type, DefaultSwapFLComponents<int>::type>  
          ls(std::move(sol), DefaultRemoveFLComponents<int>::type(), DefaultAddFLComponents<int>::type(), DefaultSwapFLComponents<int>::type());

    //search 
    search(ls);

    //print result
    auto const & s = ls.getSolution();
    auto const & ch = s.getChosenFacilities();
    std::copy(ch.begin(), ch.end(), std::ostream_iterator<int>(std::cout,","));
    std::cout << std::endl;
//! [FL Search Example]

    return true;
    
}
