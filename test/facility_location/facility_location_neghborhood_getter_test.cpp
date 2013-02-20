define BOOST_TEST_MODULE fl_neighborhood
#include <iterator>

#include <boost/test/unit_test.hpp>
#include "paal/data_structures/facility_location_solution.hpp"
#include "paal/local_search/facility_location/facility_location_solution_adapter.hpp"
#include "paal/local_search/facility_location/facility_location_neighbor_getter.hpp"
#include "utils/sample_graph.hpp"


using namespace paal::data_structures;
using namespace paal::local_search::facility_location;


BOOST_AUTO_TEST_CASE(FacilityLocationSolutionAdapterTest) {
    typedef SampleGraphsMetrics SGM;
    auto gm = SGM::getGraphMetricSmall();
    std::vector<int> fcosts{7,8};
    auto cost = [&](int i){ return fcosts[i];};

    typedef Voronoi<decltype(gm)> VorType;
    typedef typename VorType::GeneratorsSet FSet;
    VorType voronoi(FSet{SGM::A}, FSet{SGM::A,SGM::B,SGM::C,SGM::D,SGM::E} , gm);
    typedef FacilityLocationSolution<decltype(cost), VorType> Sol;
    Sol sol(std::move(voronoi), FSet{SGM::B}, cost);

   FacilityLocationSolutionAdapter<Sol> sa(sol);  
   FacilityLocationNeighborhoodGetter<int> ng;
   auto r = ng.get(sa, SolutionElement<int>(CHOSEN, SGM::A));
   auto b = r.first;
   auto e = r.second;
   BOOST_CHECK_EQUAL(std::distance(b, e), 2);

   BOOST_CHECK(b!=e);
   BOOST_CHECK_EQUAL(b->getImpl()->getType(), REMOVE);
   BOOST_CHECK(++b!=e);
   BOOST_CHECK_EQUAL(b->getImpl()->getType(), SWAP);
   BOOST_CHECK(++b == e);
}
