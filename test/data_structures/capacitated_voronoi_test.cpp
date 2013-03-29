#define BOOST_TEST_MODULE voronoi

#include <boost/test/unit_test.hpp>
#include "paal/data_structures/voronoi/capacitated_voronoi.hpp"
#include "paal/data_structures/voronoi/voronoi_traits.hpp"
#include "utils/sample_graph.hpp"
#include "utils/logger.hpp"

using namespace paal::data_structures;

std::vector<int> fcapv{7, 7};
auto fcap = [&](int i){ return fcapv[i];};

std::vector<int> cdemv{2, 2, 1, 3, 3};
auto cdem = [&](int i){ return cdemv[i];};
    

BOOST_AUTO_TEST_CASE(GeneratorLocationVoronoiutionWithClientAssignmentsTest) {
    LOG("Test 0");
    typedef SampleGraphsMetrics SGM;
    auto gm = SGM::getGraphMetricSmall();

    typedef CapacitatedVoronoi<decltype(gm), decltype(fcap), decltype(cdem)> Voronoi;
    typedef typename Voronoi::Generators GSet;
    typedef typename Voronoi::Vertices VSet;
    Voronoi voronoi(GSet{},
            VSet{SGM::A,SGM::B,SGM::C,SGM::D,SGM::E}, gm, fcap, cdem);

/*    LOG("ODLEGLOSCI OD A");
    for(int i = 0; i < 5; ++i)
        LOG(gm(SGM::A, i));
    
    LOG("ODLEGLOSCI OD B");
    for(int i = 0; i < 5; ++i)
        LOG(gm(SGM::B, i));*/

    voronoi.addGenerator(SGM::A);
    auto ab_min_a = voronoi.addGenerator(SGM::B);
    auto b_min_ab = voronoi.remGenerator(SGM::A);
    BOOST_CHECK_EQUAL(voronoi.addGenerator(SGM::A), -b_min_ab );
    BOOST_CHECK_EQUAL(voronoi.remGenerator(SGM::B), -ab_min_a);

}

BOOST_AUTO_TEST_CASE(test_1) {
    LOG("Test 1");
    typedef SampleGraphsMetrics SGM;
    auto gm = SGM::getGraphMetricSmall();

    typedef CapacitatedVoronoi<decltype(gm), decltype(fcap), decltype(cdem)> Voronoi;
    typedef paal::data_structures::VoronoiTraits<Voronoi> VT;
    typedef typename VT::GeneratorsSet GSet;
    typedef typename VT::VerticesSet VSet;
    Voronoi voronoi(GSet{SGM::A, SGM::B}, 
            VSet{SGM::A,SGM::B,SGM::C,SGM::D,SGM::E}, gm, fcap, cdem);

    voronoi.addGenerator(SGM::C);
    voronoi.remGenerator(SGM::A);
    voronoi.addGenerator(SGM::A);
    voronoi.remGenerator(SGM::C);
    voronoi.addGenerator(SGM::D);
    voronoi.remGenerator(SGM::A);
}
