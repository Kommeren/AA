/**
 * @file steiner_network_test.cpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-06-24
 */

#define BOOST_TEST_MODULE steiner_network

#include <vector>

#include <boost/test/unit_test.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "paal/iterative_rounding/iterative_rounding.hpp"
#include "paal/iterative_rounding/steiner_network/steiner_network.hpp"

#include "utils/logger.hpp"

using namespace  paal;
using namespace  paal::ir;

typedef  boost::property < boost::edge_weight_t, int> EdgeProp;

typedef boost::adjacency_list < boost::vecS, boost::vecS, boost::undirectedS,
                            boost::no_property,  EdgeProp> Graph;


int restrictions(int i, int j) {
    return 2;
}


BOOST_AUTO_TEST_CASE(steiner_network_test) {
    //sample problem
    Graph g(3);
    bool b;
    b = boost::add_edge(0 , 1, EdgeProp(1), g).second;
    assert(b);
    b = boost::add_edge(0 , 1, EdgeProp(1), g).second;
    assert(b);
    b = boost::add_edge(1 , 2, EdgeProp(1), g).second;
    assert(b);
    b = boost::add_edge(1 , 2, EdgeProp(1), g).second;
    assert(b);
    b = boost::add_edge(2 , 0, EdgeProp(7), g).second;
    assert(b);

    auto cost = boost::get(boost::edge_weight, g);

    //solve it
    auto steiner = make_SteinerNetwork(g, cost, restrictions);

    IterativeRounding<decltype(steiner)> ir(std::move(steiner));

    solve(ir);

    // printing result
    LOG("Edges in steiner network");
    auto const & edges = ir.getSolution();
    for(auto const  & e : edges) {
        LOG("Edge " << e);
    }
}

