#include <boost/test/unit_test.hpp>
#include "paal/data_structures/facility_location/k_median_solution.hpp"
#include "paal/data_structures/voronoi/voronoi.hpp"
#include "paal/local_search/facility_location/facility_location_solution_adapter.hpp"
#include "utils/sample_graph.hpp"


using namespace paal::data_structures;
using namespace paal::local_search::facility_location;


BOOST_AUTO_TEST_CASE(KMedianSolutionAdapterTest) {
    typedef SampleGraphsMetrics SGM;
    auto gm = SGM::getGraphMetricSmall();

    const int k = 2;
    typedef Voronoi<decltype(gm)> Voronoi;
    typedef paal::data_structures::VoronoiTraits<Voronoi> VT;
    typedef typename VT::GeneratorsSet GSet;
    typedef typename VT::VerticesSet VSet;
    Voronoi voronoi(GSet{SGM::A, SGM::B}, VSet{SGM::A,SGM::B,SGM::C,SGM::D,SGM::E}, gm);

    typedef paal::data_structures::KMedianSolution< Voronoi> Sol;

    Sol sol(std::move(voronoi), GSet{SGM:: C,SGM::D, SGM::E},k);
    FacilityLocationSolutionAdapter<Sol> sa(sol);
    auto & realSol = sa.getFacilityLocationSolution();

    realSol.addFacility(SGM::C);
    realSol.remFacility(SGM::A);
    realSol.addFacility(SGM::A);
    realSol.remFacility(SGM::C);
    realSol.addFacility(SGM::D);
    realSol.remFacility(SGM::A);
}
