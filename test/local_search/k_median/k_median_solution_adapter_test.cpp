#include "utils/sample_graph.hpp"

#include "paal/data_structures/facility_location/k_median_solution.hpp"
#include "paal/data_structures/voronoi/voronoi.hpp"
#include "paal/local_search/facility_location/facility_location_solution_adapter.hpp"

#include <boost/test/unit_test.hpp>



using namespace paal::data_structures;
using namespace paal::local_search::facility_location;


BOOST_AUTO_TEST_CASE(k_median_solutionAdapterTest) {
    typedef sample_graphs_metrics SGM;
    auto gm = SGM::get_graph_metric_small();

    const int k = 2;
    typedef voronoi<decltype(gm)> voronoi;
    typedef paal::data_structures::voronoi_traits<voronoi> VT;
    typedef typename VT::GeneratorsSet GSet;
    typedef typename VT::VerticesSet VSet;
    voronoi vor(GSet{SGM::A, SGM::B}, VSet{SGM::A,SGM::B,SGM::C,SGM::D,SGM::E}, gm);

    typedef paal::data_structures::k_median_solution< voronoi> Sol;

    Sol sol(std::move(vor), GSet{SGM:: C,SGM::D, SGM::E},k);
    facility_location_solution_adapter<Sol> sa(sol);
    auto & realSol = sa.getfacility_location_solution();

    realSol.add_facility(SGM::C);
    realSol.rem_facility(SGM::A);
    realSol.add_facility(SGM::A);
    realSol.rem_facility(SGM::C);
    realSol.add_facility(SGM::D);
    realSol.rem_facility(SGM::A);
}
