
#include "paal/local_search/facility_location/facility_location.hpp"
#include "utils/sample_graph.hpp"

using namespace paal::local_search::facility_location;

int main() {
    // sample data
    typedef SampleGraphsMetrics SGM;
    auto gm = SGM::getGraphMetricSmall();
    std::vector<int> fcosts{7,8};
    auto cost = [&](int i){ return fcosts[i];};
    
    //define voronoi 
    typedef paal::data_structures::Voronoi<decltype(gm)> VorType;
    typedef paal::data_structures::ObjectWithCopy<paal::data_structures::FacilityLocationSolution
        <decltype(cost), VorType>> Sol;
    typedef typename VorType::GeneratorsSet FSet;

    //create voronoi with current
    VorType voronoi(FSet{}, FSet{SGM::A,SGM::B,SGM::C,SGM::D,SGM::E}, gm);

    //create facility location local searhc step
    FacilityLocationLocalSearchStep<VorType, decltype(cost)>  
        ls(voronoi, cost, FSet{SGM::A, SGM::B});

    //search 
    search(ls);

    //print result
    Sol & s = ls.getSolution();
    auto const & ch = s->getChosenFacilities();
    std::copy(ch.begin(), ch.end(), std::ostream_iterator<int>(std::cout,","));
    std::cout << std::endl;

    return true;
    
}
