//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file bounded_degree_mst_long_test.cpp
 * @brief
 * @author Piotr Godlewski
 * @version 1.0
 * @date 2013-06-10
 */

#include "test_utils/logger.hpp"
#include "test_utils/read_bounded_deg_mst.hpp"

#include "paal/data_structures/components/components_replace.hpp"
#include "paal/iterative_rounding/bounded_degree_min_spanning_tree/bounded_degree_mst.hpp"
#include "paal/iterative_rounding/iterative_rounding.hpp"
#include "paal/utils/functors.hpp"
#include "paal/utils/irange.hpp"
#include "paal/utils/parse_file.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/test/unit_test.hpp>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS,
                              boost::property<boost::vertex_index_t, int>,
                              boost::property<boost::edge_weight_t, double>>
    Graph;
typedef boost::adjacency_list_traits<boost::vecS, boost::vecS,
                                     boost::undirectedS> Traits;
typedef boost::graph_traits<Graph>::edge_descriptor Edge;
typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;

typedef boost::property_map<Graph, boost::edge_weight_t>::type Cost;
typedef std::set<Edge> ResultTree;

template <typename Bound>
void check_result(const Graph & g, const ResultTree & tree,
                 const Cost & costs, const Bound & deg_bounds,
                 int vertices_num, double best_cost, double tree_cost) {
    int tree_edges(tree.size());
    double result_cost = std::accumulate(tree.begin(), tree.end(), 0.,
                        [&](double cost, Edge e){return cost + costs[e];});
    BOOST_CHECK_EQUAL(result_cost, tree_cost);

    LOGLN("tree edges: " << tree_edges);
    BOOST_CHECK(tree_edges == vertices_num - 1);
    BOOST_CHECK(tree_cost <= best_cost);

    auto verts = vertices(g);
    int num_of_violations(0);

    for (const Vertex & v : boost::make_iterator_range(verts.first, verts.second)) {
        int tree_deg(0);
        auto adj_vertices = adjacent_vertices(v, g);
        for (Vertex u : boost::make_iterator_range(adj_vertices)) {
            bool b; Edge e;
            std::tie(e, b) = boost::edge(v, u, g);
            assert(b);

            if (tree.count(e)) {
                ++tree_deg;
            }
        }

        BOOST_CHECK(tree_deg <= deg_bounds(v) + 1);
        if (tree_deg > deg_bounds(v)) {
            ++num_of_violations;
        }
    }

    LOGLN("Found cost = " << tree_cost << ", cost upper bound = " << best_cost);
    LOGLN("Number of violated constraints = " << num_of_violations);

    Graph tree_g(vertices_num);

    for (auto e : tree) {
        add_edge(source(e, g), target(e, g), tree_g);
    }

    std::vector<int> component(vertices_num);
    BOOST_CHECK(connected_components(tree_g, &component[0]) == 1);
}

template <typename Oracle, typename Bound>
void run_test(const Graph & g, const Cost & costs, const Bound & deg_bounds,
             const int vertices_num, const double best_cost) {
    namespace ir = paal::ir;
    {
        LOGLN("Unlimited relaxations");
        ResultTree tree;
        auto result = ir::bounded_degree_mst_iterative_rounding<Oracle>(
                            g, deg_bounds, std::inserter(tree, tree.end()));
        BOOST_CHECK(result.first == paal::lp::OPTIMAL);
        check_result(g, tree, costs, deg_bounds, vertices_num, best_cost, *(result.second));
    }
    {
        LOGLN("Relaxations limit = 1/iter");
        ResultTree tree;
        ir::bdmst_ir_components<> comps;
        auto components = paal::data_structures::replace<ir::RelaxationsLimit>(
                            ir::relaxations_limit_condition(), comps);
        auto result = ir::bounded_degree_mst_iterative_rounding<Oracle>(
                            g, deg_bounds, std::inserter(tree, tree.end()), components);
        BOOST_CHECK(result.first == paal::lp::OPTIMAL);
        check_result(g, tree, costs, deg_bounds, vertices_num, best_cost, *(result.second));
    }
}

BOOST_AUTO_TEST_CASE(bounded_degree_mst_long) {
    std::string test_dir = "test/data/BOUNDED_DEGREE_MST/";
    paal::parse(test_dir + "bdmst.txt", [&](const std::string & fname, std::istream & is_test_cases) {
        int vertices_num, edges_num;
        double best_cost;
        is_test_cases >> vertices_num >> edges_num;

        LOGLN(fname);
        std::ifstream ifs(test_dir + "/cases/" + fname + ".lgf");

        Graph g(vertices_num);
        Cost costs = get(boost::edge_weight, g);
        std::vector<int> deg_bounds(vertices_num);

        paal::read_bdmst(ifs, vertices_num, edges_num, g, costs,
                        deg_bounds, best_cost);
        auto bounds = paal::utils::make_array_to_functor(deg_bounds);

        // default heuristics
        for (int i : paal::irange(5)) {
            LOGLN("random violated, seed " << i);
            srand(i);
            run_test<paal::lp::random_violated_separation_oracle>(
                            g, costs, bounds, vertices_num, best_cost);
        }

        // non-default heuristics
        if (vertices_num <= 80) {
            LOGLN("most violated");
            run_test<paal::lp::max_violated_separation_oracle>(
                            g, costs, bounds, vertices_num, best_cost);
        }

        // non-default heuristics
        if (vertices_num <= 60) {
            LOGLN("first violated");
            run_test<paal::lp::first_violated_separation_oracle>(
                            g, costs, bounds, vertices_num, best_cost);
        }
    });
}
