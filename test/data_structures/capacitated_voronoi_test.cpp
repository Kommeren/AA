//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================

#include "test_utils/sample_graph.hpp"
#include "test_utils/logger.hpp"

#include "paal/data_structures/voronoi/capacitated_voronoi.hpp"
#include "paal/data_structures/object_with_copy.hpp"
#include "paal/data_structures/voronoi/voronoi_traits.hpp"

#include <boost/test/unit_test.hpp>

using namespace paal::data_structures;

std::vector<long long> fcapv{ 7, 7, 3, 1, 2, 2, 2, 1 };
int fcap(int i) { return fcapv[i]; }

std::vector<long long> cdemv{ 2, 2, 1, 3, 3, 1, 1, 2 };
int cdem(int i) { return cdemv[i]; }

BOOST_AUTO_TEST_SUITE(capacitated_voronoi_test_suite)

BOOST_AUTO_TEST_CASE(GeneratorLocationvoronoiutionWithClientAssignmentsTest) {
    LOGLN("Test 0");
    typedef sample_graphs_metrics SGM;
    auto gm = SGM::get_graph_metric_small();

    typedef capacitated_voronoi<decltype(gm), decltype(fcap), decltype(cdem)>
        voronoi;
    typedef typename voronoi::Generators GSet;
    typedef typename voronoi::Vertices VSet;
    voronoi vor(GSet{}, VSet{ SGM::A, SGM::B, SGM::C, SGM::D, SGM::E }, gm,
                fcap, cdem);

    /*for(int i = 0; i < 5; ++i)
        LOGLN(gm(SGM::A, i));

    for(int i = 0; i < 5; ++i)
        LOGLN(gm(SGM::B, i));*/

    vor.add_generator(SGM::A);
    auto ab_min_a = vor.add_generator(SGM::B);
    auto b_min_ab = vor.rem_generator(SGM::A);
    BOOST_CHECK_EQUAL(vor.add_generator(SGM::A), -b_min_ab);
    BOOST_CHECK_EQUAL(vor.rem_generator(SGM::B), -ab_min_a);
}

BOOST_AUTO_TEST_CASE(test_1) {
    LOGLN("Test 1");
    typedef sample_graphs_metrics SGM;
    auto gm = SGM::get_graph_metric_small();

    typedef capacitated_voronoi<decltype(gm), decltype(fcap), decltype(cdem)>
        voronoi;
    typedef paal::data_structures::voronoi_traits<voronoi> VT;
    typedef typename VT::GeneratorsSet GSet;
    typedef typename VT::VerticesSet VSet;
    voronoi vor(GSet{ SGM::A, SGM::B },
                VSet{ SGM::A, SGM::B, SGM::C, SGM::D, SGM::E }, gm, fcap, cdem);

    vor.add_generator(SGM::C);
    vor.rem_generator(SGM::A);
    vor.add_generator(SGM::A);
    vor.rem_generator(SGM::C);
    vor.add_generator(SGM::D);
    vor.rem_generator(SGM::A);
}

template <typename voronoi> void rem_add(voronoi &v, int g) {
    auto back = v.rem_generator(g);
    auto ret = v.add_generator(g);
    assert(-back == ret);
}

template <typename voronoi>
void rem_add_owc(paal::data_structures::object_with_copy<voronoi> &v, int g) {
    auto back = v.invoke_on_copy(&voronoi::rem_generator, g);
    auto ret = v.invoke_on_copy(&voronoi::add_generator, g);
    assert(-back == ret);
    back = v.invoke(&voronoi::rem_generator, g);
    ret = v.invoke(&voronoi::add_generator, g);
    assert(-back == ret);
}

BOOST_AUTO_TEST_CASE(test_2) {
    LOGLN("Test 2");
    typedef sample_graphs_metrics SGM;
    auto gm = SGM::get_graph_metric_medium();

    typedef capacitated_voronoi<decltype(gm), decltype(fcap), decltype(cdem)>
        voronoi;
    typedef paal::data_structures::voronoi_traits<voronoi> VT;
    typedef typename VT::GeneratorsSet GSet;
    typedef typename VT::VerticesSet VSet;
    voronoi vor(
        GSet{ SGM::A, SGM::B, SGM::C, SGM::D, SGM::E, SGM::F, SGM::G, SGM::H },
        VSet{ SGM::A, SGM::B, SGM::C, SGM::D, SGM::E, SGM::F, SGM::G, SGM::H },
        gm, fcap, cdem);
    for (int i = 0; i < 8; ++i) {
        rem_add(vor, i);
    }
}

BOOST_AUTO_TEST_CASE(test_3) {
    LOGLN("Test 3");
    typedef sample_graphs_metrics SGM;
    auto gm = SGM::get_graph_metric_small();

    typedef capacitated_voronoi<decltype(gm), decltype(fcap), decltype(cdem)>
        voronoi;
    typedef paal::data_structures::voronoi_traits<voronoi> VT;
    typedef typename VT::GeneratorsSet GSet;
    typedef typename VT::VerticesSet VSet;
    voronoi vor(GSet{ SGM::A /*, SGM::B*/ /*, SGM::C, SGM::D, SGM::E*/ },
                VSet{ SGM::A, SGM::B, SGM::C, SGM::D, SGM::E }, gm, fcap, cdem);

    voronoi vCopy(vor);

    for (int i = 0; i < 1 /*5*/; ++i) {
        rem_add(vCopy, i);
    }
}

BOOST_AUTO_TEST_CASE(test_4) {
    LOGLN("Test 4");
    typedef sample_graphs_metrics SGM;
    auto gm = SGM::get_graph_metric_medium();

    typedef capacitated_voronoi<decltype(gm), decltype(fcap), decltype(cdem)>
        voronoi;
    typedef paal::data_structures::voronoi_traits<voronoi> VT;
    typedef typename VT::GeneratorsSet GSet;
    typedef typename VT::VerticesSet VSet;
    voronoi vor(
        GSet{ SGM::A, SGM::B, SGM::C, SGM::D, SGM::E, SGM::F, SGM::G, SGM::H },
        VSet{ SGM::A, SGM::B, SGM::C, SGM::D, SGM::E, SGM::F, SGM::G, SGM::H },
        gm, fcap, cdem);
    for (int i = 0; i < 8; ++i) {
        paal::data_structures::object_with_copy<voronoi> owc(vor);
        rem_add_owc(owc, i);
    }
}
BOOST_AUTO_TEST_SUITE_END()
