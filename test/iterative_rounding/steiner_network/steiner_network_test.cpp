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

typedef boost::property<boost::edge_index_t, std::size_t,
            boost::property <boost::edge_weight_t, int>> EdgeProp;

typedef boost::adjacency_list <boost::vecS, boost::vecS, boost::undirectedS,
                    boost::no_property, EdgeProp> Graph;

typedef boost::graph_traits<Graph>::edge_descriptor Edge;
typedef std::vector<Edge> ResultNetwork;

int restrictions(int i, int j) {
    return 2;
}

BOOST_AUTO_TEST_SUITE(steiner_network)
BOOST_AUTO_TEST_CASE(steiner_network_test) {
    //sample problem
    LOGLN("Sample problem:");
    Graph g(3);
    ResultNetwork resultNetwork;
    bool b;
    b  = add_edge(0, 1, EdgeProp(0, 1), g).second;
    b &= add_edge(0, 1, EdgeProp(1, 1), g).second;
    b &= add_edge(1, 2, EdgeProp(2, 1), g).second;
    b &= add_edge(1, 2, EdgeProp(3, 1), g).second;
    b &= add_edge(2, 0, EdgeProp(4, 7), g).second;
    assert(b);

    //solve it
    auto oracle(make_SteinerNetworkSeparationOracle(g, restrictions));
    SteinerNetworkIRComponents<Graph, decltype(restrictions), ResultNetwork>
                comps(lp::make_RowGenerationSolveLP(oracle));
    steiner_network_iterative_rounding(g, restrictions,
                std::back_inserter(resultNetwork), std::move(comps));

    // printing result
    LOGLN("Edges in steiner network");
    ON_LOG(for(auto const  & e : resultNetwork) {
        LOGLN("Edge " << e);
    })
}

BOOST_AUTO_TEST_CASE(steiner_network_test_properties) {
    //sample problem
    LOGLN("Sample problem:");
    Graph g(3);

    bool b;
    b  = add_edge(0, 1, 0, g).second;
    b &= add_edge(0, 1, 1, g).second;
    b &= add_edge(1, 2, 2, g).second;
    b &= add_edge(1, 2, 3, g).second;
    b &= add_edge(2, 0, 4, g).second;
    assert(b);

    auto edgeId = get(boost::edge_index, g);
    std::vector<double> costs = {1, 1, 1, 1, 7};
    auto cost = boost::make_iterator_property_map(costs.begin(), edgeId);

    //solve it
    {
        ResultNetwork resultNetwork;
        auto oracle(make_SteinerNetworkSeparationOracle(g, restrictions));
        SteinerNetworkIRComponents<Graph, decltype(restrictions), ResultNetwork>
                    comps(lp::make_RowGenerationSolveLP(oracle));
        steiner_network_iterative_rounding(g, restrictions, boost::weight_map(cost),
                    std::back_inserter(resultNetwork), std::move(comps));

        // printing result
        LOGLN("Edges in steiner network");
        ON_LOG(for(auto const  & e : resultNetwork) {
            LOGLN("Edge " << e);
        })
    }
    {
        ResultNetwork resultNetwork;
        auto oracle(make_SteinerNetworkSeparationOracle(g, restrictions));
        SteinerNetworkIRComponents<Graph, decltype(restrictions), ResultNetwork>
                    comps(lp::make_RowGenerationSolveLP(oracle));
        auto steinerNetwork(make_SteinerNetwork(g, restrictions,
                    boost::weight_map(cost),
                    std::back_inserter(resultNetwork)));
        solve_iterative_rounding(steinerNetwork, std::move(comps));

        // printing result
        LOGLN("Edges in steiner network");
        ON_LOG(for(auto const  & e : resultNetwork) {
            LOGLN("Edge " << e);
        })
    }
}

BOOST_AUTO_TEST_CASE(steiner_network_invalid_test) {
    // invalid problem (restrictions cannot be satisfied)
    LOGLN("Invalid problem (restrictions cannot be satisfied):");
    Graph g(3);
    typedef boost::graph_traits<Graph>::edge_descriptor Edge;
    typedef std::vector<Edge> ResultNetwork;
    ResultNetwork resultNetwork;
    bool b;
    b  = add_edge(0, 1, EdgeProp(0, 1), g).second;
    b &= add_edge(1, 2, EdgeProp(1, 1), g).second;
    assert(b);

    //solve it
    auto oracle(make_SteinerNetworkSeparationOracle(g, restrictions));
    SteinerNetworkIRComponents<Graph, decltype(restrictions), ResultNetwork>
                comps(lp::make_RowGenerationSolveLP(oracle));
    auto steinerNetwork(make_SteinerNetwork(g, restrictions,
                                    std::back_inserter(resultNetwork)));
    auto invalid = steinerNetwork.checkInputValidity();

    BOOST_CHECK(invalid);
    LOGLN(*invalid);
}
BOOST_AUTO_TEST_SUITE_END()

