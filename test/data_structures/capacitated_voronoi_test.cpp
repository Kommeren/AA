
#include <boost/test/unit_test.hpp>
#include "paal/data_structures/voronoi/capacitated_voronoi.hpp"
#include "paal/data_structures/object_with_copy.hpp"
#include "paal/data_structures/voronoi/voronoi_traits.hpp"
#include "utils/sample_graph.hpp"
#include "utils/logger.hpp"

using namespace paal::data_structures;

std::vector<long long> fcapv{7, 7, 3, 1, 2, 2, 2, 1};
auto fcap = [&](int i){ return fcapv[i];};

std::vector<long long> cdemv{2, 2, 1, 3, 3, 1, 1, 2};
auto cdem = [&](int i){ return cdemv[i];};


BOOST_AUTO_TEST_SUITE( capacitated_voronoi_test_suite )

BOOST_AUTO_TEST_CASE(GeneratorLocationVoronoiutionWithClientAssignmentsTest) {
    LOGLN("Test 0");
    typedef SampleGraphsMetrics SGM;
    auto gm = SGM::getGraphMetricSmall();

    typedef CapacitatedVoronoi<decltype(gm), decltype(fcap), decltype(cdem)> Voronoi;
    typedef typename Voronoi::Generators GSet;
    typedef typename Voronoi::Vertices VSet;
    Voronoi voronoi(GSet{},
            VSet{SGM::A,SGM::B,SGM::C,SGM::D,SGM::E}, gm, fcap, cdem);

    /*for(int i = 0; i < 5; ++i)
        LOGLN(gm(SGM::A, i));

    for(int i = 0; i < 5; ++i)
        LOGLN(gm(SGM::B, i));*/

    voronoi.addGenerator(SGM::A);
    auto ab_min_a = voronoi.addGenerator(SGM::B);
    auto b_min_ab = voronoi.remGenerator(SGM::A);
    BOOST_CHECK_EQUAL(voronoi.addGenerator(SGM::A), -b_min_ab );
    BOOST_CHECK_EQUAL(voronoi.remGenerator(SGM::B), -ab_min_a);

}

BOOST_AUTO_TEST_CASE(test_1) {
    LOGLN("Test 1");
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

template <typename Voronoi>
void remAdd(Voronoi & v, int g) {
    auto back = v.remGenerator(g);
    auto ret = v.addGenerator(g);
    assert(-back == ret);
}

template <typename Voronoi>
void remAddOWC(paal::data_structures::ObjectWithCopy<Voronoi> & v, int g) {
    auto back = v.invokeOnCopy(&Voronoi::remGenerator, g);
    auto ret = v.invokeOnCopy(&Voronoi::addGenerator, g);
    assert(-back == ret);
    back = v.invoke(&Voronoi::remGenerator, g);
    ret = v.invoke(&Voronoi::addGenerator, g);
    assert(-back == ret);
}

BOOST_AUTO_TEST_CASE(test_2) {
    LOGLN("Test 2");
    typedef SampleGraphsMetrics SGM;
    auto gm = SGM::getGraphMetricMedium();

    typedef CapacitatedVoronoi<decltype(gm), decltype(fcap), decltype(cdem)> Voronoi;
    typedef paal::data_structures::VoronoiTraits<Voronoi> VT;
    typedef typename VT::GeneratorsSet GSet;
    typedef typename VT::VerticesSet VSet;
    Voronoi voronoi(GSet{SGM::A, SGM::B, SGM::C, SGM::D, SGM::E, SGM::F, SGM::G, SGM::H},
                    VSet{SGM::A, SGM::B, SGM::C, SGM::D, SGM::E, SGM::F, SGM::G, SGM::H}, gm, fcap, cdem);
    for(int i = 0;i  < 8; ++i) {
        remAdd(voronoi, i);
    }
}

BOOST_AUTO_TEST_CASE(test_3) {
    LOGLN("Test 3");
    typedef SampleGraphsMetrics SGM;
    auto gm = SGM::getGraphMetricSmall();

    typedef CapacitatedVoronoi<decltype(gm), decltype(fcap), decltype(cdem)> Voronoi;
    typedef paal::data_structures::VoronoiTraits<Voronoi> VT;
    typedef typename VT::GeneratorsSet GSet;
    typedef typename VT::VerticesSet VSet;
    Voronoi voronoi(GSet{SGM::A/*, SGM::B*//*, SGM::C, SGM::D, SGM::E*/},
                    VSet{SGM::A, SGM::B, SGM::C, SGM::D, SGM::E}, gm, fcap, cdem);

    Voronoi vCopy(voronoi);

    for(int i = 0;i  < 1/*5*/; ++i) {
        remAdd(vCopy, i);
    }
}

BOOST_AUTO_TEST_CASE(test_4) {
    LOGLN("Test 4");
    typedef SampleGraphsMetrics SGM;
    auto gm = SGM::getGraphMetricMedium();

    typedef CapacitatedVoronoi<decltype(gm), decltype(fcap), decltype(cdem)> Voronoi;
    typedef paal::data_structures::VoronoiTraits<Voronoi> VT;
    typedef typename VT::GeneratorsSet GSet;
    typedef typename VT::VerticesSet VSet;
    Voronoi voronoi(GSet{SGM::A, SGM::B, SGM::C, SGM::D, SGM::E, SGM::F, SGM::G, SGM::H},
                    VSet{SGM::A, SGM::B, SGM::C, SGM::D, SGM::E, SGM::F, SGM::G, SGM::H}, gm, fcap, cdem);
    for(int i = 0;i  < 8; ++i) {
        paal::data_structures::ObjectWithCopy<Voronoi> owc(voronoi);
        remAddOWC(owc, i);
    }
}
BOOST_AUTO_TEST_SUITE_END()
