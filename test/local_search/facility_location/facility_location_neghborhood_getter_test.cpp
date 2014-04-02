#include <boost/test/unit_test.hpp>
#include "paal/data_structures/facility_location/facility_location_solution.hpp"
#include "paal/local_search/facility_location/facility_location_solution_adapter.hpp"
#include "paal/local_search/facility_location/facility_location_add.hpp"
#include "paal/local_search/facility_location/facility_location_remove.hpp"
#include "paal/local_search/facility_location/facility_location_swap.hpp"
#include "utils/sample_graph.hpp"


using namespace paal::data_structures;
using namespace paal::local_search::facility_location;


BOOST_AUTO_TEST_CASE(FacilityLocationRemoveTest) {
    typedef sample_graphs_metrics SGM;
    auto gm = SGM::get_graph_metric_small();
    std::vector<int> fcosts{7,8};
    auto cost = [&](int i){ return fcosts[i];};

    typedef voronoi<decltype(gm)> VorType;
    typedef typename VorType::GeneratorsSet FSet;
    VorType voronoi(FSet{SGM::A}, FSet{SGM::A,SGM::B,SGM::C,SGM::D,SGM::E} , gm);
    typedef facility_location_solution<decltype(cost), VorType> Sol;
    Sol sol(std::move(voronoi), FSet{SGM::B}, cost);
    facility_location_solution_adapter<Sol> sa(sol);

    {
        facility_locationget_moves_remove ng;
        auto r = ng(sa);
        auto b = std::begin(r);
        auto e = std::end(r);
        BOOST_CHECK_EQUAL(std::distance(b, e), 1);
    }
    {
        facility_locationget_moves_add ng;
        auto r = ng(sa);
        auto b = std::begin(r);
        auto e = std::end(r);
        BOOST_CHECK_EQUAL(std::distance(b, e), 1);
    }
    {
        facility_locationget_moves_swap ng;
        auto r = ng(sa);
        auto b = std::begin(r);
        auto e = std::end(r);
        BOOST_CHECK_EQUAL(std::distance(b, e), 1);
    }
}
