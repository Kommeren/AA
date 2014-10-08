//=======================================================================
// Copyright (c) 2013 Piotr Wygocki
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file zelikovsky_11_per_6_test.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-04
 */
#include <boost/test/unit_test.hpp>

// this include must be here! //hack for clang
#include "paal/steiner_tree/zelikovsky_11_per_6.hpp"

#include "utils/logger.hpp"
#include "utils/sample_graph.hpp"

#include <iosfwd>

namespace std {
std::ostream &operator<<(std::ostream &o, const std::pair<int, int> &p) {
    o << p.first << "," << p.second;
    return o;
}
}

BOOST_AUTO_TEST_CASE(zelikowsky_test) {
    using SGM = sample_graphs_metrics;
    auto gm = SGM::get_graph_metric_steiner();
    using Metric = decltype(gm);
    using voronoiT = paal::data_structures::voronoi<Metric>;
    using FSet = typename voronoiT::GeneratorsSet;
    voronoiT voronoi(FSet{ SGM::A, SGM::B, SGM::C, SGM::D }, FSet{ SGM::E },
                     gm);

    std::vector<int> steiner;

    paal::steiner_tree_zelikovsky11per6approximation(
        gm, voronoi, std::back_inserter(steiner));
    BOOST_CHECK_EQUAL(steiner.size(), std::size_t(1));
    BOOST_CHECK_EQUAL(steiner.front(), SGM::E);
}

BOOST_AUTO_TEST_CASE(zelikowsky_euclidean_metric_test) {
    using EMetric = paal::data_structures::euclidean_metric<int>;
    using VoronoiT = paal::data_structures::voronoi<EMetric>;
    using FSet = typename VoronoiT::GeneratorsSet;

    auto test_case = sample_graphs_metrics::get_euclidean_steiner_sample<FSet>();
    VoronoiT voronoi(std::get<1>(test_case), std::get<2>(test_case), std::get<0>(test_case));

    std::vector<std::pair<int, int>> steiner;

    paal::steiner_tree_zelikovsky11per6approximation(
        std::get<0>(test_case), voronoi, std::back_inserter(steiner));
    BOOST_CHECK_EQUAL(steiner.size(), std::size_t(0));
}
