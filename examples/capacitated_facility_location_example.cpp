
#include "paal/local_search/facility_location/facility_location.hpp"
#include "paal/data_structures/voronoi/capacitated_voronoi.hpp"
#include "utils/sample_graph.hpp"

using namespace paal::local_search::facility_location;

int main() {
//! [FL Search Example]
    // sample data
    typedef SampleGraphsMetrics SGM;
    auto gm = SGM::getGraphMetricSmall();
   
    std::vector<int> fcosts{7,8};
    auto cost = [&](int i){ return fcosts[i];};

    std::vector<std::vector<int>> capv{{},{},{},{},{}};
    auto cap = [&](int i, int j){ return capv[i][j];};

    std::vector<int> fcapv{};
    auto fcap = [&](int i){ return fcapv[i];};
    
    std::vector<int> cdemv{};
    auto cdem = [&](int i){ return cdemv[i];};
    
    //define voronoi and solution
    typedef paal::data_structures::CapacitatedVoronoi<
        decltype(gm), decltype(cap), decltype(fcap), decltype(cdem)> VorType;
    typedef paal::data_structures::FacilityLocationSolution
        <decltype(cost), VorType> Sol;
    typedef paal::data_structures::ObjectWithCopy<Sol> SolOcjWithCopy;
    typedef paal::data_structures::VoronoiTraits<VorType> VT;
    typedef typename VT::GeneratorsSet GSet;
    typedef typename VT::VerticesSet VSet;
    typedef typename Sol::UnchosenFacilitiesSet USet;


    //create voronoi and solution
    VorType voronoi(GSet{}, VSet{SGM::A,SGM::B,SGM::C,SGM::D,SGM::E}, gm, cap, fcap, cdem);
    Sol sol(std::move(voronoi), USet{SGM::A, SGM::B}, cost);

    //create facility location local search step
    FacilityLocationLocalSearchStep<VorType, decltype(cost)>  
        ls(std::move(sol));

    //search 
    search(ls);

    //print result
    SolOcjWithCopy & s = ls.getSolution();
    auto const & ch = s->getChosenFacilities();
    std::copy(ch.begin(), ch.end(), std::ostream_iterator<int>(std::cout,","));
    std::cout << std::endl;
//! [FL Search Example]

    return true;
    
}
