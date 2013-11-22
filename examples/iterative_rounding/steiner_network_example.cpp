/**
 * @file steiner_network_example.cpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-06-24
 */

#include <vector>

#include <boost/graph/adjacency_list.hpp>

#include "paal/iterative_rounding/iterative_rounding.hpp"
#include "paal/iterative_rounding/steiner_network/steiner_network.hpp"

using namespace  paal;
using namespace  paal::ir;

typedef  boost::property < boost::edge_weight_t, int> EdgeProp;

typedef boost::adjacency_list < boost::vecS, boost::vecS, boost::undirectedS,
                            boost::no_property,  EdgeProp> Graph;


int restrictions(int i, int j) {
    return 2;
}


int main() {
    Graph g(3);
    typedef boost::graph_traits<Graph>::edge_descriptor Edge;
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

    typedef std::set<Edge> ResultNetwork;
    ResultNetwork resultNetwork;
    auto cost = boost::get(boost::edge_weight, g);

    //solve it
    auto oracle(make_SteinerNetworkSeparationOracle(g, restrictions, resultNetwork));
    SteinerNetworkIRComponents<Graph, decltype(restrictions), ResultNetwork> comps(lp::make_RowGenerationSolveLP(oracle));
    steiner_network_iterative_rounding(g, restrictions, cost, resultNetwork, std::move(comps));

    // printing result
    std::cout << "Edges in steiner network" << std::endl;
    for(auto const  & e : resultNetwork) {
        std::cout << "Edge " << e << std::endl;
    }
    paal::lp::GLP::freeEnv();
}

