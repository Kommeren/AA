#define BOOST_TEST_MODULE voronoi

#include <boost/test/unit_test.hpp>
#include "paal/data_structures/voronoi/voronoi.hpp"
#include "utils/sample_graph.hpp"
#include "utils/logger.hpp"

using namespace paal::data_structures;


BOOST_AUTO_TEST_CASE(GeneratorLocationVoronoiutionWithClientAssignmentsTest) {
    LOG("Test 0");
    typedef SampleGraphsMetrics SGM;
    auto gm = SGM::getGraphMetricSmall();

    typedef Voronoi<decltype(gm)> Voronoi;
    typedef typename Voronoi::GeneratorsSet FSet;
    Voronoi voronoi(FSet{},
            FSet{SGM::A,SGM::B,SGM::C,SGM::D,SGM::E}, gm);

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

    typedef Voronoi<decltype(gm)> Voronoi;
    typedef paal::data_structures::VoronoiTraits<Voronoi> VT;
    typedef typename VT::GeneratorsSet GSet;
    typedef typename VT::VerticesSet VSet;
    Voronoi voronoi(GSet{SGM::A, SGM::B}, VSet{SGM::A,SGM::B,SGM::C,SGM::D,SGM::E}, gm);

    voronoi.addGenerator(SGM::C);
    voronoi.remGenerator(SGM::A);
    voronoi.addGenerator(SGM::A);
    voronoi.remGenerator(SGM::C);
    voronoi.addGenerator(SGM::D);
    voronoi.remGenerator(SGM::A);
}
