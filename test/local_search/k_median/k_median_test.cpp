#include <boost/test/unit_test.hpp>

#include "paal/local_search/k_median/k_median.hpp"
#include "utils/sample_graph.hpp"
#include "utils/logger.hpp"


BOOST_AUTO_TEST_CASE(kmedian_test) {
    // sample data
    typedef SampleGraphsMetrics SGM;
    auto gm = SGM::getGraphMetricSmall();

    //define voronoi and solution
    const int k = 2;
    typedef paal::data_structures::Voronoi<decltype(gm)> VorType;
    typedef paal::data_structures::KMedianSolution
        <VorType> Sol;
    typedef paal::data_structures::VoronoiTraits<VorType> VT;
    typedef typename VT::GeneratorsSet GSet;
    typedef typename VT::VerticesSet VSet;
    typedef typename Sol::UnchosenFacilitiesSet USet;


    //create voronoi and solution
    VorType voronoi(GSet{SGM::B, SGM::D}, VSet{SGM::A,SGM::B,SGM::C,SGM::D,SGM::E}, gm);
    Sol sol(std::move(voronoi), USet{SGM::A, SGM::C}, k);

    //create facility location local search components
    paal::local_search::k_median::DefaultKMedianComponents<int>::type swap;

    //search
    paal::local_search::facility_location::facility_location_local_search_simple(sol, swap);

    //print result
    ON_LOG(auto const & ch = )sol.getChosenFacilities();
    LOGLN("Solution:");
    LOG_COPY_RANGE_DEL(ch, ",");
}
