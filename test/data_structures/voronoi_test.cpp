#define BOOST_TEST_MODULE cycle_manager

#include <boost/test/unit_test.hpp>
#include "data_structures/voronoi.hpp"
#include "utils/sample_graph.hpp"

using namespace paal::data_structures;


BOOST_AUTO_TEST_CASE(GeneratorLocationVoronoiutionWithClientAssignmentsTest) {
    typedef SampleGraphsMetrics SGM;
    auto gm = SGM::getGraphMetricSmall();

    typedef Voronoi<int, decltype(gm)> Voronoi;
    typedef typename Voronoi::GeneratorsSet FSet;
    Voronoi voronoi(FSet{},
            FSet{SGM::A,SGM::B,SGM::C,SGM::D,SGM::E}, gm);

    voronoi.addGenerator(SGM::A);
    auto ab_min_a = voronoi.addGenerator(SGM::B);
    auto b_min_ab = voronoi.remGenerator(SGM::A);
    BOOST_CHECK_EQUAL(voronoi.addGenerator(SGM::A), -b_min_ab );
    BOOST_CHECK_EQUAL(voronoi.remGenerator(SGM::B), -ab_min_a);

}
