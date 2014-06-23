/**
 * @file dreyfus_wagner_test.cpp
 * @brief
 * @author Maciej Andrejczuk
 * @version 1.0
 * @date 2013-08-04
 */
#include "utils/sample_graph.hpp"

#include "paal/steiner_tree/dreyfus_wagner.hpp"

#include <boost/test/unit_test.hpp>

using namespace paal;
using namespace paal::steiner_tree;

BOOST_AUTO_TEST_CASE(testBasic) {
    typedef sample_graphs_metrics SGM;
    auto gm = SGM::get_graph_metric_steiner();
    std::vector<int> terminals = { SGM::A, SGM::B, SGM::C, SGM::D };
    std::vector<int> nonterminals = { SGM::E };
    auto dw = make_dreyfus_wagner(gm, terminals, nonterminals);
    dw.solve();
    BOOST_CHECK_EQUAL(dw.get_cost(), 4);
    BOOST_CHECK_EQUAL(
        int(dw.steiner_tree_zelikovsky11per6approximation().size()), 1);
    BOOST_CHECK_EQUAL(dw.get_edges().size(), 4);
}

BOOST_AUTO_TEST_CASE(testMedium) {
    typedef sample_graphs_metrics SGM;
    auto gm = SGM::get_graph_metric_medium();
    std::vector<int> terminals = { SGM::A, SGM::B, SGM::C, SGM::F, SGM::G,
                                   SGM::H };
    std::vector<int> nonterminals = { SGM::D, SGM::E };
    auto dw = make_dreyfus_wagner(gm, terminals, nonterminals);
    dw.solve();
    BOOST_CHECK_EQUAL(dw.get_cost(), 15);
    BOOST_CHECK_EQUAL(dw.steiner_tree_zelikovsky11per6approximation().size(),
                      0);
    BOOST_CHECK_EQUAL(dw.get_edges().size(), 5);
}
