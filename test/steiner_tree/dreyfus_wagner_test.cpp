/**
 * @file dreyfus_wagner_test.cpp
 * @brief 
 * @author Maciej Andrejczuk
 * @version 1.0
 * @date 2013-08-04
 */
#include <boost/test/unit_test.hpp>

#include "paal/steiner_tree/dreyfus_wagner.hpp"
#include "utils/sample_graph.hpp"

using namespace paal;
using namespace paal::steiner_tree;

BOOST_AUTO_TEST_CASE(testBasic) {
    typedef  SampleGraphsMetrics SGM;
    auto gm = SGM::getGraphMetricSteiner();
    std::vector<int> terminals = {SGM::A, SGM::B, SGM::C, SGM::D};
    std::vector<int> nonterminals = {SGM::E};
    auto dw = make_DreyfusWagner(gm, terminals, nonterminals);
    dw.solve();
    BOOST_CHECK_EQUAL(dw.getCost(), 4);
    BOOST_CHECK_EQUAL(int(dw.steinerTreeZelikovsky11per6approximation().size()), 1);
    BOOST_CHECK_EQUAL(dw.getEdges().size(), 4);
}

BOOST_AUTO_TEST_CASE(testMedium) {
    typedef  SampleGraphsMetrics SGM;
    auto gm = SGM::getGraphMetricMedium();
    std::vector<int> terminals = {SGM::A, SGM::B, SGM::C, SGM::F, SGM::G, SGM::H};
    std::vector<int> nonterminals = {SGM::D, SGM::E};
    auto dw = make_DreyfusWagner(gm, terminals, nonterminals);
    dw.solve();
    BOOST_CHECK_EQUAL(dw.getCost(), 15);
    BOOST_CHECK_EQUAL(dw.steinerTreeZelikovsky11per6approximation().size(), 0);
    BOOST_CHECK_EQUAL(dw.getEdges().size(), 5);
}
