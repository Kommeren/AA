#define BOOST_TEST_MODULE facility_location_solution_adapter

#include <boost/test/unit_test.hpp>
#include "data_structures/facility_location_solution.hpp"
#include "local_search/facility_location/facility_location_solution_adapter.hpp"
#include "utils/sample_graph.hpp"


using namespace paal::data_structures;
using namespace paal::local_search::facility_location;


BOOST_AUTO_TEST_CASE(FacilityLocationSolutionAdapterTest) {
    typedef SampleGraphsMetrics SGM;
    auto gm = SGM::getGraphMetric();
    std::vector<int> fcosts{7,8};
    auto cost = [&](int i){ return fcosts[i];};

    typedef FacilityLocationSolutionWithClientsAssignment
        <int, decltype(gm), decltype(cost)> Sol;
    
    typedef typename Sol::FacilitiesSet FSet;
    Sol sol(FSet{SGM::A,SGM::B}, FSet{},
            FSet{SGM::A,SGM::B,SGM::C,SGM::D,SGM::E}, gm, cost);

   FacilityLocationSolutionAdapter<Sol> sa(std::move(sol));  
   auto b = sa.begin();
   auto e = sa.end();
   BOOST_CHECK_EQUAL(*b, SolutionElement<int>(UNCHOSEN, SGM::A));
   BOOST_CHECK(b != e);
   BOOST_CHECK_EQUAL(*(++b), SolutionElement<int>(UNCHOSEN, SGM::B));
   BOOST_CHECK(b != e);
   BOOST_CHECK(++b == e);

   auto & realSol = sa.get(); 

   realSol.invoke(&Sol::addFacility,SGM::A);
   
   b = sa.begin();
   e = sa.end();
   BOOST_CHECK_EQUAL(*b, SolutionElement<int>(CHOSEN, SGM::A));
   BOOST_CHECK(b != e);
   BOOST_CHECK_EQUAL(*(++b), SolutionElement<int>(UNCHOSEN, SGM::B));
   BOOST_CHECK(b != e);
   BOOST_CHECK(++b == e);


}
