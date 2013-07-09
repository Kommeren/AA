#define BOOST_TEST_MODULE facility_location_solution_adapter

#include <boost/test/unit_test.hpp>
#include "paal/local_search/k_median/k_median_solution.hpp"
#include "paal/data_structures/voronoi/voronoi.hpp"
#include "paal/local_search/facility_location/facility_location_solution_adapter.hpp"
#include "utils/sample_graph.hpp"


using namespace paal::data_structures;
using namespace paal::local_search::facility_location;


BOOST_AUTO_TEST_CASE(FacilityLocationSolutionAdapterTest) {
    typedef SampleGraphsMetrics SGM;
    auto gm = SGM::getGraphMetricSmall();
   
    const int k = 2;
    typedef Voronoi<decltype(gm)> Voronoi;
    typedef paal::data_structures::VoronoiTraits<Voronoi> VT;
    typedef typename VT::GeneratorsSet GSet;
    typedef typename VT::VerticesSet VSet;
    Voronoi voronoi(GSet{SGM::A, SGM::B}, VSet{SGM::A,SGM::B,SGM::C,SGM::D,SGM::E}, gm);

    typedef paal::local_search::k_median::KMedianSolution<k, Voronoi> Sol;
    
    Sol sol(std::move(voronoi), GSet{SGM:: C,SGM::D, SGM::E});
    FacilityLocationSolutionAdapter<Sol> sa(std::move(sol));  
    auto & realSol = sa.get(); 

    realSol.invoke(&Sol::addFacility, SGM::C);
    realSol.invoke(&Sol::remFacility, SGM::A);
    realSol.invoke(&Sol::addFacility, SGM::A);
    realSol.invoke(&Sol::remFacility, SGM::C);
    realSol.invoke(&Sol::addFacility, SGM::D);
    realSol.invoke(&Sol::remFacility, SGM::A);
}
