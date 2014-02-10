#include <boost/test/unit_test.hpp>
#include "paal/local_search/facility_location/facility_location.hpp"
#include "paal/local_search/custom_components.hpp"
#include "paal/utils/functors.hpp"
#include "utils/sample_graph.hpp"
#include "utils/logger.hpp"

using namespace paal::local_search::facility_location;


BOOST_AUTO_TEST_CASE(FacilityLocationTest) {
    
    typedef SampleGraphsMetrics SGM;
    auto gm = SGM::getGraphMetricSmall();
    std::vector<int> fcosts{7,8};
    auto cost = paal::utils::make_ArrayToFunctor(fcosts);
    
    typedef paal::data_structures::Voronoi<decltype(gm)> VorType;
    typedef paal::data_structures::FacilityLocationSolution
        <decltype(cost), VorType> Sol;
    typedef typename VorType::GeneratorsSet FSet;
    VorType voronoi(FSet{}, FSet{SGM::A,SGM::B,SGM::C,SGM::D,SGM::E}, gm);
    Sol sol(std::move(voronoi),FSet{SGM::A, SGM::B}, cost );
    DefaultRemoveFLComponents<int>::type rem;
    DefaultAddFLComponents<int>::type    add;
    DefaultSwapFLComponents<int>::type   swap;
    paal::utils::SkipFunctor nop;
    //this search can and in one or 2 rounds depending on
    //implementation of unordered_set in FacilityLocationSolution
    //if the first facility to add is A, then the search will be finished 
    //in  one round. The 2 round are needed otherwise
    paal::local_search::StopConditionCountLimit oneRoundSearch(2);
    
    ON_LOG(auto const & ch = sol.getChosenFacilities());
    LOGLN("Solution before the first search");
    LOG_COPY_RANGE_DEL(ch, ",");
    LOGLN("");

    BOOST_CHECK(facility_location_local_search(
                    sol, nop, oneRoundSearch, rem, add, swap));
    
    LOGLN("Solution after the first search");
    LOG_COPY_RANGE_DEL(ch, ",");
    LOGLN("");
    BOOST_CHECK(!facility_location_local_search_simple(sol, rem, add ,swap)); 
}
