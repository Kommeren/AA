//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file dreyfus_wagner_test.cpp
 * @brief
 * @author Maciej Andrejczuk, Piotr Wygocki
 * @version 1.0
 * @date 2013-08-04
 */

#include "utils/sample_graph.hpp"

#include "paal/steiner_tree/dreyfus_wagner.hpp"

#include <boost/test/unit_test.hpp>

using namespace paal;

BOOST_AUTO_TEST_SUITE(dreyfus_wagner)

BOOST_AUTO_TEST_CASE(test_basic) {
    typedef sample_graphs_metrics SGM;
    auto gm = SGM::get_graph_metric_steiner();
    std::vector<int> terminals = { SGM::A, SGM::B, SGM::C, SGM::D };
    std::vector<int> nonterminals = { SGM::E };
    auto dw = make_dreyfus_wagner(gm, terminals, nonterminals);
    dw.solve();
    BOOST_CHECK_EQUAL(dw.get_cost(), 4);
    BOOST_CHECK_EQUAL(int(dw.get_steiner_elements().size()), 1);
    BOOST_CHECK_EQUAL(dw.get_edges().size(), 4);
}

BOOST_AUTO_TEST_CASE(test_medium) {
    typedef sample_graphs_metrics SGM;
    auto gm = SGM::get_graph_metric_medium();
    std::vector<int> terminals = { SGM::A, SGM::B, SGM::C, SGM::F, SGM::G,
                                   SGM::H };
    std::vector<int> nonterminals = { SGM::D, SGM::E };
    auto dw = make_dreyfus_wagner(gm, terminals, nonterminals);
    dw.solve();
    BOOST_CHECK_EQUAL(dw.get_cost(), 15);
    BOOST_CHECK_EQUAL(dw.get_steiner_elements().size(), 0);
    BOOST_CHECK_EQUAL(dw.get_edges().size(), 5);
}

BOOST_AUTO_TEST_CASE(euclidean_metric_test) {
    using Points = std::vector<std::pair<int, int>>;

    data_structures::euclidean_metric<int> em;
    Points terminals, nonterminals;
    std::tie(em, terminals, nonterminals) = sample_graphs_metrics::get_euclidean_steiner_sample();

    auto dw = make_dreyfus_wagner(em, terminals, nonterminals);
    dw.solve();

    BOOST_CHECK_EQUAL(dw.get_steiner_elements().size(), std::size_t(1));
    BOOST_CHECK(*dw.get_steiner_elements().begin() == std::make_pair(1, 1));
    BOOST_CHECK_EQUAL(dw.get_cost(), 4);
}

BOOST_AUTO_TEST_SUITE_END()
