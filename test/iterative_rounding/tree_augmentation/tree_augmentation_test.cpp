/**
 * @file tree_augmentation_test.cpp
 * @brief
 * @author Attila Bernath, Piotr Godlewski
 * @version 1.0
 * @date 2013-07-10
 */

#include <boost/test/unit_test.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "utils/logger.hpp"
#include "paal/iterative_rounding/treeaug/tree_augmentation.hpp"

using namespace  paal;
using namespace  paal::ir;
using namespace  boost;

struct LogVisitor : public TrivialVisitor {

    template <typename LP>
    void roundCol(LP & lp, int col, double val) {
        LOGLN("Column "<< col << " rounded to " << val);
    }

    template <typename LP>
    void relaxRow(LP & lp, int row) {
        LOGLN("Relax row " << row);
    }
};

// create a typedef for the Graph type
typedef adjacency_list<vecS, vecS, undirectedS,
            no_property,
            property < edge_weight_t, double,
            property < edge_color_t, bool> > > Graph;

typedef adjacency_list_traits < vecS, vecS, undirectedS > Traits;
typedef graph_traits < Graph >::edge_descriptor Edge;

template <typename Graph, typename TreeMap, typename Cost>
Edge addEdge(Graph & g,  int u, int v,
        TreeMap tree, bool inT,
        Cost & cost, double c) {
    bool b;
    Traits::edge_descriptor e;
    std::tie(e, b) = add_edge(u, v, g);
    assert(b);
    tree[e]=inT;
    cost[e] = c;
    return e;
}

typedef property_map < Graph, vertex_index_t >::type Index;
typedef property_map < Graph, edge_weight_t >::type Cost;
typedef property_map < Graph, edge_color_t >::type TreeMap;

BOOST_AUTO_TEST_SUITE(tree_augmentation) 

BOOST_AUTO_TEST_CASE(tree_augmentation_test) {
    // sample problem
    LOGLN("Sample problem:");
    Graph g(6);
    Cost cost = get(edge_weight, g);
    TreeMap treeMap = get(edge_color, g);

    addEdge(g, 0, 1, treeMap, true, cost, 0);
    addEdge(g, 1, 2, treeMap, true, cost, 0);
    addEdge(g, 1, 3, treeMap, true, cost, 0);
    addEdge(g, 3, 4, treeMap, true, cost, 0);
    addEdge(g, 3, 5, treeMap, true, cost, 0);
    addEdge(g, 0, 3, treeMap, false, cost, 1);
    addEdge(g, 0, 2, treeMap, false, cost, 1);
    addEdge(g, 2, 4, treeMap, false, cost, 1);
    addEdge(g, 2, 5, treeMap, false, cost, 1);
    addEdge(g, 4, 5, treeMap, false, cost, 1);

    typedef std::set<Edge> SolutionTree;
    SolutionTree solutionTree;

    auto treeaug(make_TreeAug(g, treeMap, cost, std::inserter(solutionTree, solutionTree.begin())));
    paal::ir::solve_iterative_rounding(treeaug, paal::ir::TAComponents<>());
    BOOST_CHECK(!solutionTree.empty());
}

BOOST_AUTO_TEST_CASE(tree_augmentation_invalid_test_1) {
    // invalid problem (wrong number of spanning tree edges)
    LOGLN("Invalid problem (wrong number of spanning tree edges):");
    Graph g(6);
    Cost cost = get(edge_weight, g);
    TreeMap treeMap = get(edge_color, g);

    addEdge(g, 0, 1, treeMap, true, cost, 0);
    addEdge(g, 1, 2, treeMap, true, cost, 0);
    addEdge(g, 1, 3, treeMap, false, cost, 0);
    addEdge(g, 3, 4, treeMap, false, cost, 0);
    addEdge(g, 3, 5, treeMap, false, cost, 0);

    typedef std::vector<Edge> SolutionTree;
    SolutionTree solutionTree;

    auto treeaug(make_TreeAug(g, treeMap, cost, std::back_inserter(solutionTree)));
    auto invalid = treeaug.checkInputValidity();

    BOOST_CHECK(invalid);
    LOGLN(*invalid);
}

BOOST_AUTO_TEST_CASE(tree_augmentation_invalid_test_2) {
    // invalid problem (spanning tree not connected)
    LOGLN("Invalid problem (spanning tree not connected):");
    Graph g(6);
    Cost cost = get(edge_weight, g);
    TreeMap treeMap = get(edge_color, g);

    addEdge(g, 0, 1, treeMap, true, cost, 0);
    addEdge(g, 1, 2, treeMap, true, cost, 0);
    addEdge(g, 1, 3, treeMap, false, cost, 0);
    addEdge(g, 3, 4, treeMap, true, cost, 0);
    addEdge(g, 3, 5, treeMap, true, cost, 0);
    addEdge(g, 1, 4, treeMap, false, cost, 0);
    addEdge(g, 5, 4, treeMap, true, cost, 0);
    addEdge(g, 2, 3, treeMap, false, cost, 0);

    typedef std::vector<Edge> SolutionTree;
    SolutionTree solutionTree;

    auto treeaug(make_TreeAug(g, treeMap, cost, std::back_inserter(solutionTree)));
    auto invalid = treeaug.checkInputValidity();

    BOOST_CHECK(invalid);
    LOGLN(*invalid);
}

BOOST_AUTO_TEST_CASE(tree_augmentation_invalid_test_3) {
    // invalid problem (graph not 2-edge-connected)
    LOGLN("Invalid problem (graph not 2-edge-connected):");
    Graph g(6);
    Cost cost = get(edge_weight, g);
    TreeMap treeMap = get(edge_color, g);

    addEdge(g, 0, 1, treeMap, true, cost, 0);
    addEdge(g, 1, 2, treeMap, true, cost, 0);
    addEdge(g, 1, 3, treeMap, true, cost, 0);
    addEdge(g, 3, 4, treeMap, true, cost, 0);
    addEdge(g, 3, 5, treeMap, true, cost, 0);
    addEdge(g, 1, 4, treeMap, false, cost, 0);
    addEdge(g, 5, 4, treeMap, false, cost, 0);
    addEdge(g, 2, 3, treeMap, false, cost, 0);

    typedef std::vector<Edge> SolutionTree;
    SolutionTree solutionTree;

    auto treeaug(make_TreeAug(g, treeMap, cost, std::back_inserter(solutionTree)));
    auto invalid = treeaug.checkInputValidity();

    BOOST_CHECK(invalid);
    LOGLN(*invalid);
}

BOOST_AUTO_TEST_SUITE_END() 
