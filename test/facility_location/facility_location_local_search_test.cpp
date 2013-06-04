#define BOOST_TEST_MODULE facility_location_local_search

#include <iterator>

#include <boost/test/unit_test.hpp>
#include "paal/local_search/facility_location/facility_location.hpp"
#include "utils/sample_graph.hpp"
#include "utils/logger.hpp"

using namespace paal::local_search::facility_location;

BOOST_AUTO_TEST_CASE(FacilityLocationSolutionTest) {
    
    typedef SampleGraphsMetrics SGM;
    auto gm = SGM::getGraphMetricSmall();
    std::vector<int> fcosts{7,8};
    auto cost = [&](int i){ return fcosts[i];};
    
    typedef paal::data_structures::Voronoi<decltype(gm)> VorType;
    typedef paal::data_structures::FacilityLocationSolution
        <decltype(cost), VorType> Sol;
    typedef typename VorType::GeneratorsSet FSet;
    VorType voronoi(FSet{}, FSet{SGM::A,SGM::B,SGM::C,SGM::D,SGM::E}, gm);
    Sol sol(std::move(voronoi),FSet{SGM::A, SGM::B}, cost );

    FacilityLocationLocalSearchStep<VorType, decltype(cost)>  
        ls(std::move(sol));

    LOG("cost(0) = " << cost(0));
    BOOST_CHECK(ls.search());
    ON_LOG(auto & s = ls.getSolution());
    ON_LOG(auto const & ch = s->getChosenFacilities());
    LOG_COPY_DEL(ch.begin(), ch.end(), ",");
    LOG("");
    BOOST_CHECK(!ls.search());
    
}
