//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
#include "utils/sample_graph.hpp"
#include "utils/logger.hpp"

#include "paal/data_structures/voronoi/voronoi.hpp"

#include <boost/test/unit_test.hpp>

using namespace paal::data_structures;

BOOST_AUTO_TEST_CASE(GeneratorLocationvoronoiutionWithClientAssignmentsTest) {
    LOGLN("Test 0");
    typedef sample_graphs_metrics SGM;
    auto gm = SGM::get_graph_metric_small();

    typedef voronoi<decltype(gm)> voronoi;
    typedef typename voronoi::GeneratorsSet FSet;
    voronoi vor(FSet{}, FSet{ SGM::A, SGM::B, SGM::C, SGM::D, SGM::E }, gm);

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

    typedef voronoi<decltype(gm)> voronoi;
    typedef paal::data_structures::voronoi_traits<voronoi> VT;
    typedef typename VT::GeneratorsSet GSet;
    typedef typename VT::VerticesSet VSet;
    voronoi vor(GSet{ SGM::A, SGM::B },
                VSet{ SGM::A, SGM::B, SGM::C, SGM::D, SGM::E }, gm);

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

BOOST_AUTO_TEST_CASE(test_2) {
    LOGLN("Test 2");
    typedef sample_graphs_metrics SGM;
    auto gm = SGM::get_graph_metric_small();

    typedef voronoi<decltype(gm)> voronoi;
    typedef paal::data_structures::voronoi_traits<voronoi> VT;
    typedef typename VT::GeneratorsSet GSet;
    typedef typename VT::VerticesSet VSet;
    voronoi vor(GSet{ SGM::A, SGM::B, SGM::C, SGM::D, SGM::E },
                VSet{ SGM::A, SGM::B, SGM::C, SGM::D, SGM::E }, gm);
    rem_add(vor, SGM::A);
    rem_add(vor, SGM::B);
    rem_add(vor, SGM::C);
    rem_add(vor, SGM::D);
    rem_add(vor, SGM::E);
}

BOOST_AUTO_TEST_CASE(test_make_voronoi) {
    LOGLN("Test 1");
    typedef sample_graphs_metrics SGM;
    auto gm = SGM::get_graph_metric_small();

    typedef voronoi<decltype(gm)> voronoi;
    typedef paal::data_structures::voronoi_traits<voronoi> VT;
    typedef typename VT::GeneratorsSet GSet;
    typedef typename VT::VerticesSet VSet;
    voronoi vor(GSet{ SGM::A, SGM::B },
                VSet{ SGM::A, SGM::B, SGM::C, SGM::D, SGM::E }, gm);

    BOOST_CHECK(vor == make_voronoi(GSet{ SGM::A, SGM::B }, VSet{ SGM::A, SGM::B, SGM::C, SGM::D, SGM::E }, gm));
}
