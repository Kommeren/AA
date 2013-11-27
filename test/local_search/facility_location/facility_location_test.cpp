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
    paal::local_search::StopConditionCountLimit oneRoundSearch(1);

    BOOST_CHECK(facility_location_local_search(
                    sol, nop, oneRoundSearch, rem, add, swap));
    
    ON_LOG(auto const & ch = sol.getChosenFacilities());
    LOGLN("Solution after the first search");
    LOG_COPY_RANGE_DEL(ch, ",");
    LOGLN("");
    BOOST_CHECK(!facility_location_local_search_simple(sol, rem, add ,swap)); 
}
