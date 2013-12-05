/**
 * @file steiner_network_test.cpp
 * @brief
 * @author Piotr Wygocki, Piotr Godlewski
 * @version 1.0
 * @date 2013-06-24
 */


#include <vector>

#include <boost/test/unit_test.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "paal/iterative_rounding/iterative_rounding.hpp"
#include "paal/iterative_rounding/steiner_network/steiner_network.hpp"

#include "utils/logger.hpp"

using namespace  paal;
using namespace  paal::ir;

typedef boost::property < boost::edge_weight_t, int> EdgeProp;

typedef boost::adjacency_list < boost::vecS, boost::vecS, boost::undirectedS,
                            boost::no_property,  EdgeProp> Graph;

int restrictions(int i, int j) {
    return 2;
}

BOOST_AUTO_TEST_CASE(steiner_network_test) {
    //sample problem
    LOGLN("Sample problem:");
    Graph g(3);
    typedef boost::graph_traits<Graph>::edge_descriptor Edge;
    typedef std::vector<Edge> ResultNetwork;
    ResultNetwork resultNetwork;
    bool b;
    b = boost::add_edge(0, 1, EdgeProp(1), g).second;
    assert(b);
    b = boost::add_edge(0, 1, EdgeProp(1), g).second;
    assert(b);
    b = boost::add_edge(1, 2, EdgeProp(1), g).second;
    assert(b);
    b = boost::add_edge(1, 2, EdgeProp(1), g).second;
    assert(b);
    b = boost::add_edge(2, 0, EdgeProp(7), g).second;
    assert(b);

    auto cost = get(boost::edge_weight, g);

    //solve it
    auto oracle(make_SteinerNetworkSeparationOracle(g, restrictions));
    SteinerNetworkIRComponents<Graph, decltype(restrictions), ResultNetwork>
                comps(lp::make_RowGenerationSolveLP(oracle));
    steiner_network_iterative_rounding(g, restrictions, cost,
                std::back_inserter(resultNetwork), std::move(comps));

    // printing result
    LOGLN("Edges in steiner network");
    ON_LOG(for(auto const  & e : resultNetwork) {
        LOGLN("Edge " << e);
    })
}

BOOST_AUTO_TEST_CASE(steiner_network_invalid_test) {
    // invalid problem (restrictions cannot be satisfied)
    LOGLN("Invalid problem (restrictions cannot be satisfied):");
    Graph g(3);
    typedef boost::graph_traits<Graph>::edge_descriptor Edge;
    typedef std::vector<Edge> ResultNetwork;
    ResultNetwork resultNetwork;
    bool b;
    b = boost::add_edge(0, 1, EdgeProp(1), g).second;
    assert(b);
    b = boost::add_edge(1, 2, EdgeProp(1), g).second;
    assert(b);

    auto cost = get(boost::edge_weight, g);

    //solve it
    auto oracle(make_SteinerNetworkSeparationOracle(g, restrictions));
    SteinerNetworkIRComponents<Graph, decltype(restrictions), ResultNetwork>
                comps(lp::make_RowGenerationSolveLP(oracle));
    auto steinerNetwork(make_SteinerNetwork(g, restrictions, cost,
                                    std::back_inserter(resultNetwork)));
    auto invalid = steinerNetwork.checkInputValidity();

    BOOST_CHECK(invalid);
    LOGLN(*invalid);
}

