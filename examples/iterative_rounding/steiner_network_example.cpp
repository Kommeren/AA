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
    //sample problem
//    auto  restrictions = [&](int i, int j){return 2;};
    
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

    solve_iterative_rounding(ir);

    // printing result
    std::cout << "Edges in steiner network" << std::endl;
    auto const & edges = ir.getSolution();
    for(auto const  & e : edges) {
        std::cout << "Edge " << e << std::endl;
    }
}

