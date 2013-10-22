/**
 * @file tree_augmentation_example.cpp
 * @brief 
 * @author Attila Bernath, Piotr Godlewski
 * @version 1.0
 * @date 2013-10-17
 */
#include <iostream>
#include <set>

#include "paal/iterative_rounding/treeaug/tree_augmentation.hpp"

typedef boost::property<boost::edge_weight_t, double,
                     boost::property<boost::edge_color_t, bool>> EdgeProp;

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS,
                            boost::no_property, EdgeProp> Graph;

int main() {
//! [Tree Augmentation Example]
    // sample problem
    Graph g(6);
    typedef boost::graph_traits<Graph>::edge_descriptor Edge;
    bool b;
    b =  add_edge(0, 1, EdgeProp(0, 1), g).second;
    b &= add_edge(1, 2, EdgeProp(0, 1), g).second;
    b &= add_edge(1, 3, EdgeProp(0, 1), g).second;
    b &= add_edge(3, 4, EdgeProp(0, 1), g).second;
    b &= add_edge(3, 5, EdgeProp(0, 1), g).second;
    b &= add_edge(0, 3, EdgeProp(1, 0), g).second;
    b &= add_edge(0, 2, EdgeProp(1, 0), g).second;
    b &= add_edge(2, 4, EdgeProp(1, 0), g).second;
    b &= add_edge(2, 5, EdgeProp(1, 0), g).second;
    b &= add_edge(4, 5, EdgeProp(1, 0), g).second;
    assert(b);

    typedef std::set<Edge> EdgeSet;
    EdgeSet solution;
    auto cost = get(boost::edge_weight, g);
    auto treeMap = get(boost::edge_color, g);

    // optional input validity checking
    auto treeAug = paal::ir::make_TreeAug(g, treeMap, cost, solution);
    auto error = treeAug.checkInputValidity();
    if (error) {
        std::cerr << "The input is not valid!" << std::endl;
        std::cerr << *error << std::endl;
        return -1;
    }

    // solve it
    paal::ir::tree_augmentation_iterative_rounding(g, treeMap, cost, solution,
                                    paal::ir::TAComponents<>());

    std::cout << "The solution contains the following nontree edges:" << std::endl;

    for (auto const & e : solution) {
        std::cout << "Edge " << e << std::endl;
    }
//! [Tree Augmentation Example]
    //TODO change after adding of cost computation to the IR
//    std::cout<<"The total cost of the solution is "<<treeaug.getSolutionValue()<<std::endl;
    paal::lp::GLP::freeEnv();

    return 0;
}

