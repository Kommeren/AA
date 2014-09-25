//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file tree_augmentation_long_test.cpp
 * @brief
 * @author Attila Bernath
 * @version 1.0
 * @date 2013-07-08
 */


#include "utils/logger.hpp"
#include "utils/parse_file.hpp"
#include "utils/test_result_check.hpp"

#include "paal/iterative_rounding/treeaug/tree_augmentation.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/test/unit_test.hpp>

#include <fstream>

using namespace paal;
using namespace paal::ir;
using namespace boost;


using Graph = adjacency_list<vecS, vecS, undirectedS,
        no_property,
        property<edge_weight_t, double,
        property<edge_color_t, bool>>>;

using Traits = adjacency_list_traits<vecS, vecS, undirectedS>;
using Vertex = graph_traits<Graph>::vertex_descriptor;
using Edge = graph_traits<Graph>::edge_descriptor;

using Index = property_map<Graph, vertex_index_t>::type;
using Cost = property_map<Graph, edge_weight_t>::type;
using TreeMap = property_map<Graph, edge_color_t>::type;

// Read instance in format
// @nodes 6
// label
// 0
// 1
// 2
// 3
// 4
// 5
// @edges 10
//                 label   intree  cost
// 0       1       0       1       0
// 1       2       1       1       0
// 1       3       2       1       0
// 3       4       3       1       0
// 3       5       4       1       0
// 0       3       0       0       1
// 0       2       1       0       1
// 2       4       2       0       1
// 2       5       3       0       1
// 4       5       4       0       1
void read_tree_aug(std::ifstream & is,
        Graph & g, Cost & cost, TreeMap & tree_map) {
    std::string s;
    std::unordered_map<std::string, Vertex> nodes;
    int vertices_num, edges_num;
    is >> s; is >> vertices_num; is >> s;

    for (int i = 0; i < vertices_num; i++) {
        std::string nlabel;
        is >> nlabel;
        nodes[nlabel] = add_vertex(g);
    }

    LOGLN(num_vertices(g));

    is >> s; is >> edges_num; is >> s; is >> s; is >> s;

    for (int i = 0; i < edges_num; i++) {
        // read from the file
        std::string u, v;
        double c;
        int dummy;
        bool in_t;
        is >> u >> v >> dummy >> in_t >> c;

        bool b;
        Traits::edge_descriptor e;
        std::tie(e, b) = add_edge(nodes[u], nodes[v], g);
        assert(b);
        put(cost, e, c);
        put(tree_map, e, in_t);
    }
}

template <typename TA>
// the copy is intended
    double get_lower_bound(TA ta) {
    tree_augmentation_ir_components<> comps;
    lp::glp lp;
    comps.call<Init>(ta, lp);
    auto prob_type = comps.call<SolveLP>(ta, lp);
    BOOST_CHECK_EQUAL(prob_type, lp::OPTIMAL);
    return lp.get_obj_value();
}

BOOST_AUTO_TEST_CASE(tree_augmentation_long) {
    std::string test_dir = "test/data/TREEAUG/";
    parse(test_dir + "tree_aug.txt", [&](const std::string & fname, std::istream &) {
        LOGLN(fname);
        std::string filename = test_dir + "cases/" + fname + ".lgf";
        std::ifstream ifs(filename);

        if (!ifs) {
            std::cerr << "File " << filename << " could not be opened."
                      << std::endl;
            return;
        }

        Graph g;
        Cost cost = get(edge_weight, g);
        TreeMap tree_map = get(edge_color, g);

        read_tree_aug(ifs, g, cost, tree_map);

        std::vector<Edge> solution;
        auto treeaug(make_tree_aug(g, std::back_inserter(solution)));

        auto invalid = treeaug.check_input_validity();
        BOOST_CHECK(!invalid);
        LOGLN("Input validation " << filename << " ends.");

        double lplowerbd = get_lower_bound(treeaug);
        auto result = solve_iterative_rounding(
            treeaug, tree_augmentation_ir_components<>());
        BOOST_CHECK_EQUAL(result.first, lp::OPTIMAL);

        double solval = treeaug.get_solution_cost();
        BOOST_CHECK_EQUAL(solval, *(result.second));
        check_result_compare_to_bound(solval, lplowerbd, 2);
    });
}
