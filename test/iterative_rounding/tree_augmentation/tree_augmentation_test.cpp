/**
 * @file tree_augmentation_test.cpp
 * @brief
 * @author Attila Bernath, Piotr Godlewski
 * @version 1.0
 * @date 2013-07-10
 */

#include "utils/logger.hpp"

#include "paal/iterative_rounding/treeaug/tree_augmentation.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/graph/adjacency_list.hpp>


using namespace paal;
using namespace paal::ir;
using namespace boost;

struct log_visitor : public trivial_visitor {

    template <typename LP>
    void round_col(LP & lp, int col, double val) {
        LOGLN("Column "<< col << " rounded to " << val);
    }

    template <typename LP>
    void relax_row(LP & lp, int row) {
        LOGLN("Relax row " << row);
    }
};

template <typename VertexList, typename EdgeProp>
using Graph = adjacency_list<vecS, VertexList, undirectedS,
                property<vertex_index_t, int>, EdgeProp>;

using EdgeProp = property<edge_index_t, std::size_t,
                    property<edge_weight_t, double,
                        property<edge_color_t, bool>>>;
using VectorGraph = Graph<vecS, EdgeProp>;
using Edge = graph_traits<VectorGraph>::edge_descriptor;

template <typename Graph, typename TreeMap, typename Cost>
Edge add_edge_to_graph(Graph & g, int u, int v,
        TreeMap tree, bool inT,
        Cost & cost, double c) {
    bool b;
    Edge e;
    std::tie(e, b) = add_edge(u, v, g);
    assert(b);
    tree[e] = inT;
    cost[e] = c;
    return e;
}

using Cost = property_map<VectorGraph, edge_weight_t>::type;
using TreeMap = property_map<VectorGraph, edge_color_t>::type;
using SolutionTree = std::set<Edge>;

BOOST_AUTO_TEST_SUITE(tree_augmentation)

BOOST_AUTO_TEST_CASE(tree_augmentation_test) {
    // sample problem
    LOGLN("Sample problem:");
    VectorGraph g(6);
    Cost cost = get(edge_weight, g);
    TreeMap treeMap = get(edge_color, g);

    add_edge_to_graph(g, 0, 1, treeMap, true, cost, 0);
    add_edge_to_graph(g, 1, 2, treeMap, true, cost, 0);
    add_edge_to_graph(g, 1, 3, treeMap, true, cost, 0);
    add_edge_to_graph(g, 3, 4, treeMap, true, cost, 0);
    add_edge_to_graph(g, 3, 5, treeMap, true, cost, 0);
    add_edge_to_graph(g, 0, 3, treeMap, false, cost, 1);
    add_edge_to_graph(g, 0, 2, treeMap, false, cost, 1);
    add_edge_to_graph(g, 2, 4, treeMap, false, cost, 1);
    add_edge_to_graph(g, 2, 5, treeMap, false, cost, 1);
    add_edge_to_graph(g, 4, 5, treeMap, false, cost, 1);

    SolutionTree solutionTree;

    auto treeaug(make_tree_aug(g, std::inserter(solutionTree, solutionTree.begin())));
    solve_iterative_rounding(treeaug, tree_augmentation_ir_components<>());
    BOOST_CHECK(!solutionTree.empty());
}

BOOST_AUTO_TEST_CASE(tree_augmentation_list) {
    // boost::listS instead of boost::vecS for vertex storage
    using EdgeProp = property<edge_weight_t, double,
                        property<edge_color_t, bool>>;
    using ListGraph = Graph<listS, EdgeProp>;
    using EdgeT = graph_traits<ListGraph>::edge_descriptor;

    std::vector<std::pair<int, int>> edges = {{0,1},{1,2},{1,3},{3,4},{3,5},{0,3},
            {0,2},{2,4},{2,5},{4,5}};
    std::vector<EdgeProp> edge_properties {EdgeProp(0,1), EdgeProp(0,1),
        EdgeProp(0,1), EdgeProp(0,1), EdgeProp(0,1), EdgeProp(1,0),
        EdgeProp(1,0), EdgeProp(1,0), EdgeProp(1,0), EdgeProp(1,0)};
    ListGraph g(edges.begin(), edges.end(), edge_properties.begin(), 6);

    auto index = get(boost::vertex_index, g);
    int idx = 0;
    for (auto v : boost::make_iterator_range(vertices(g))) {
        put(index, v, idx);
        ++idx;
    }

    std::vector<EdgeT> solutionTree;
    tree_augmentation_iterative_rounding(g, std::inserter(solutionTree, solutionTree.begin()));
    BOOST_CHECK(!solutionTree.empty());
}

BOOST_AUTO_TEST_CASE(tree_augmentation_test_parameters) {
    // sample problem
    LOGLN("Sample problem:");
    VectorGraph g(6);
    TreeMap treeMap = get(edge_color, g);

    treeMap[add_edge(0, 1, 0, g).first] = true;
    treeMap[add_edge(1, 2, 1, g).first] = true;
    treeMap[add_edge(1, 3, 2, g).first] = true;
    treeMap[add_edge(3, 4, 3, g).first] = true;
    treeMap[add_edge(3, 5, 4, g).first] = true;
    treeMap[add_edge(0, 3, 5, g).first] = false;
    treeMap[add_edge(0, 2, 6, g).first] = false;
    treeMap[add_edge(2, 4, 7, g).first] = false;
    treeMap[add_edge(2, 5, 8, g).first] = false;
    treeMap[add_edge(4, 5, 9, g).first] = false;

    auto edgeId = get(boost::edge_index, g);
    std::vector<double> costs = {0, 0, 0, 0, 0, 1, 1, 1, 1, 1};
    auto cost = boost::make_iterator_property_map(costs.begin(), edgeId);

    {
        SolutionTree solutionTree;
        auto treeaug(make_tree_aug(g, boost::edge_color_map(treeMap).weight_map(cost),
                std::inserter(solutionTree, solutionTree.begin())));
        solve_iterative_rounding(treeaug, tree_augmentation_ir_components<>());
        BOOST_CHECK(!solutionTree.empty());
    }
    {
        SolutionTree solutionTree;
        tree_augmentation_iterative_rounding(
                g, boost::weight_map(cost),
                std::inserter(solutionTree, solutionTree.begin()));
        BOOST_CHECK(!solutionTree.empty());
    }
}

void run_invalid_test(const VectorGraph & g){
    SolutionTree solutionTree;
    auto treeaug(make_tree_aug(g, std::back_inserter(solutionTree)));
    auto invalid = treeaug.check_input_validity();

    BOOST_CHECK(invalid);
    LOGLN(*invalid);
}

BOOST_AUTO_TEST_CASE(tree_augmentation_invalid_test_1) {
    // invalid problem (wrong number of spanning tree edges)
    LOGLN("Invalid problem (wrong number of spanning tree edges):");
    VectorGraph g(6);
    Cost cost = get(edge_weight, g);
    TreeMap treeMap = get(edge_color, g);

    add_edge_to_graph(g, 0, 1, treeMap, true, cost, 0);
    add_edge_to_graph(g, 1, 2, treeMap, true, cost, 0);
    add_edge_to_graph(g, 1, 3, treeMap, false, cost, 0);
    add_edge_to_graph(g, 3, 4, treeMap, false, cost, 0);
    add_edge_to_graph(g, 3, 5, treeMap, false, cost, 0);

    run_invalid_test(g);
}

BOOST_AUTO_TEST_CASE(tree_augmentation_invalid_test_2) {
    // invalid problem (spanning tree not connected)
    LOGLN("Invalid problem (spanning tree not connected):");
    VectorGraph g(6);
    Cost cost = get(edge_weight, g);
    TreeMap treeMap = get(edge_color, g);

    add_edge_to_graph(g, 0, 1, treeMap, true, cost, 0);
    add_edge_to_graph(g, 1, 2, treeMap, true, cost, 0);
    add_edge_to_graph(g, 1, 3, treeMap, false, cost, 0);
    add_edge_to_graph(g, 3, 4, treeMap, true, cost, 0);
    add_edge_to_graph(g, 3, 5, treeMap, true, cost, 0);
    add_edge_to_graph(g, 1, 4, treeMap, false, cost, 0);
    add_edge_to_graph(g, 5, 4, treeMap, true, cost, 0);
    add_edge_to_graph(g, 2, 3, treeMap, false, cost, 0);

    run_invalid_test(g);
}

BOOST_AUTO_TEST_CASE(tree_augmentation_invalid_test_3) {
    // invalid problem (graph not 2-edge-connected)
    LOGLN("Invalid problem (graph not 2-edge-connected):");
    VectorGraph g(6);
    Cost cost = get(edge_weight, g);
    TreeMap treeMap = get(edge_color, g);

    add_edge_to_graph(g, 0, 1, treeMap, true, cost, 0);
    add_edge_to_graph(g, 1, 2, treeMap, true, cost, 0);
    add_edge_to_graph(g, 1, 3, treeMap, true, cost, 0);
    add_edge_to_graph(g, 3, 4, treeMap, true, cost, 0);
    add_edge_to_graph(g, 3, 5, treeMap, true, cost, 0);
    add_edge_to_graph(g, 1, 4, treeMap, false, cost, 0);
    add_edge_to_graph(g, 5, 4, treeMap, false, cost, 0);
    add_edge_to_graph(g, 2, 3, treeMap, false, cost, 0);

    run_invalid_test(g);
}

BOOST_AUTO_TEST_SUITE_END()
