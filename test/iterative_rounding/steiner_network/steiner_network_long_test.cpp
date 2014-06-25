//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file steiner_network_long_test.cpp
 * @brief
 * @author Piotr Godlewski
 * @version 1.0
 * @date 2013-07-10
 */

#include "test_utils/logger.hpp"
#include "test_utils/read_bounded_deg_mst.hpp"

#include "paal/iterative_rounding/steiner_network/steiner_network.hpp"
#include "paal/utils/irange.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/range/join.hpp>
#include <boost/test/unit_test.hpp>

#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>
#include <utility>
#include <vector>

using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS,
                        boost::property<boost::vertex_index_t, int>,
                        boost::property<boost::edge_weight_t, double>>;
using Traits = boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::undirectedS>;
using Edge = boost::graph_traits<Graph>::edge_descriptor;
using Vertex = boost::graph_traits<Graph>::vertex_descriptor;

using Cost = boost::property_map<Graph, boost::edge_weight_t>::type;

using RestrictionVec = std::vector<std::vector<int>>;
using Edges = std::map<std::pair<int, int>, int>;

namespace {

int rand(int min, int max) { return ::rand() % (max - min + 1) + min; }

int rand_cost() {
    static const int MAX_COST = 100;
    return rand(1, MAX_COST);
}

void generate_restrictions(RestrictionVec & res, int max_res) {
    for (int i : paal::irange(res.size())) {
        for (int j : paal::irange(i+1, int(res.size()))) {
            res[i][j] = res[j][i] = rand(2, max_res);
        }
    }
}

bool non_zero(const std::vector<int> &vec) {
    for (auto v : vec)
        if (v != 0) return true;

    return false;
}

bool non_zero(const RestrictionVec &res) {
    for (auto row : res)
        if (non_zero(row)) return true;

    return false;
}

void dec(std::vector<int> &vec) {
    for (auto &v : vec) v = std::max(v - 1, 0);
}

void add_edge_to_graph(Edges & edges, int u, int v, int & edge_num) {
    auto edge = std::make_pair(std::min(u,v), std::max(u,v));
    if (edges.find(edge) == edges.end()) {
        edges.insert(std::make_pair(edge, 1));
        ++edge_num;
    }
    else {
        ++edges[edge];
        edge_num += 2;
    }
}

void random_tree(Edges & edges, RestrictionVec & restrictions,
        int special_vertices, int vertices_num, int & edge_num) {
    int first = rand(special_vertices, vertices_num - 2);
    std::vector<int> vertices(1, first);

    // Generates edge between current vertex and already processed (tree) vertices.
    auto generate_tree_edge = [&](int v){
        int index = rand(0, vertices.size() - 1);
        add_edge_to_graph(edges, v, vertices[index], edge_num);
        vertices.push_back(v);
    };

    // Generate a random tree on non-special vertices.
    for (auto v : boost::join(paal::irange(first + 1, vertices_num),
                              paal::irange(special_vertices, first))) {
        generate_tree_edge(v);
    }

    // Join special vertices with non-zero restrictions to the random tree.
    // There is be a path in the tree between every pair of vertices,
    // so we decrement special vertices restrictions.
    for (int v : paal::irange(special_vertices)) {
        if (non_zero(restrictions[v])) {
            generate_tree_edge(v);
            dec(restrictions[v]);
        }
    }
}

void fill_graph(Graph & g, const Edges & edges, int & vertices_num) {
    g.clear();
    auto cost = get(boost::edge_weight, g);
    for (auto e : edges) {
        // add regular edge
        paal::add_edge_to_graph(g, cost, e.first.first, e.first.second,
                                rand_cost());
        // add new vertices on multi-edges, so that we get a graph not a
        // multigraph
        for (int i = 1; i < e.second; ++i) {
            paal::add_edge_to_graph(g, cost, e.first.first, vertices_num, rand_cost());
            paal::add_edge_to_graph(g, cost, e.first.second, vertices_num, rand_cost());
            ++vertices_num;
        }
    }
}

struct clique_tag {};
struct sparse_graph_tag {};

int generate_instance(Graph & g, RestrictionVec & res, int vertices_num,
        int min_edge_num, int special_vertices, int max_res, unsigned int seed,
        sparse_graph_tag) {
    srand(seed);
    assert(special_vertices + 1 < vertices_num);
    assert(max_res >= 2);

    int edge_num(0);
    Edges edges;
    res = RestrictionVec(special_vertices, std::vector<int>(special_vertices, 0));

    generate_restrictions(res, max_res);
    RestrictionVec temp_res = res;

    while (non_zero(temp_res)) {
        random_tree(edges, temp_res, special_vertices, vertices_num, edge_num);
    }

    while (edge_num < min_edge_num) {
        int u = rand(0, vertices_num - 1);
        int v = rand(u + 1, vertices_num);
        add_edge_to_graph(edges, u, v, edge_num);
    }

    fill_graph(g, edges, vertices_num);

    LOGLN("edges: " << edge_num);

    return vertices_num;
}

int generate_instance(Graph & g, RestrictionVec & res, int vertices_num,
        int min_edge_num, int restricted_vertices, int max_res, unsigned int seed,
        clique_tag) {
    srand(seed);
    assert(restricted_vertices <= vertices_num);
    assert(max_res >= 2);

    res = RestrictionVec(restricted_vertices, std::vector<int>(restricted_vertices, 0));
    generate_restrictions(res, max_res);

    auto cost = get(boost::edge_weight, g);
    for (int i : paal::irange(vertices_num)) {
        for (int j : paal::irange(i+1, vertices_num)) {
            paal::add_edge_to_graph(g, cost, i, j, rand_cost());
        }
    }

    return vertices_num;
}

template <typename Oracle, typename Restrictions>
void run_single_test(const Graph &g, const Cost &costs,
                     const Restrictions &restrictions) {
    namespace ir = paal::ir;
    namespace lp = paal::lp;

    using ResultNetwork = std::vector<Edge>;
    ResultNetwork result_network;
    auto steiner_network(ir::make_steiner_network<Oracle>(g, restrictions,
                            std::back_inserter(result_network)));
    auto invalid = steiner_network.check_input_validity();
    BOOST_CHECK(!invalid);

    auto result = ir::solve_iterative_rounding(steiner_network,
                        ir::steiner_network_ir_components<>());
    BOOST_CHECK(result.first == lp::OPTIMAL);
}

template <typename Restrictions>
void run_test(const Graph & g, const Cost & costs, const Restrictions & restrictions,
        int vertices_num) {
    // default heuristics
    for (int i : paal::irange(5)) {
        LOGLN("random violated, seed " << i);
        srand(i);
        run_single_test<paal::lp::random_violated_separation_oracle>(
            g, costs, restrictions);
    }

    // non-default heuristics
    if (vertices_num <= 80) {
        LOGLN("most violated");
        run_single_test<paal::lp::max_violated_separation_oracle>(
            g, costs, restrictions);
    }

    // non-default heuristics
    if (vertices_num <= 50) {
        LOGLN("first violated");
        run_single_test<paal::lp::first_violated_separation_oracle>(
            g, costs, restrictions);
    }
}

template <typename GraphTypeTag>
void run_test_case(const std::vector<int> & vertices_num, const std::vector<int> & min_edge_num,
        const std::vector<int> & special_vertices, const std::vector<int> & max_res,
        GraphTypeTag graph_type_tag) {
    Graph g;
    RestrictionVec res;
    auto seed = [](int x) { return 12345 * x; };

    for (int i : paal::irange(vertices_num.size())) {
        int vertices = generate_instance(g, res, vertices_num[i], min_edge_num[i],
                special_vertices[i], max_res[i], seed(i), graph_type_tag);
        LOGLN("vertices: " << vertices);
        LOGLN("special vertices: " << special_vertices[i]);
        LOGLN("max restriction: " << max_res[i]);

        auto restrictions = [&](int i, int j) {
            if (std::size_t(i) < res.size() && std::size_t(j) < res.size())
                return res[i][j];
            else
                return 1;
        };

        run_test(g, get(boost::edge_weight, g), restrictions, vertices);
    }
}

} // namespace

BOOST_AUTO_TEST_SUITE(steiner_network_long)

BOOST_AUTO_TEST_CASE(steiner_network_long_sparse) {
    std::vector<int> vertices_num     = { 6, 10, 10,  10,  15,  20,  40,  50,  75};
    std::vector<int> min_edge_num     = {30, 50, 80, 200, 200, 250, 400, 350, 500};
    std::vector<int> special_vertices = { 4,  4,  6,   5,   5,  10,  15,  15,  20};
    std::vector<int> max_res          = { 3,  3,  5,   6,   7,   5,   8,   6,   6};

    run_test_case(vertices_num, min_edge_num, special_vertices, max_res, sparse_graph_tag());
}

BOOST_AUTO_TEST_CASE(steiner_network_long_clique) {
    std::vector<int> vertices_num     = {10, 20, 40, 60, 80};
    std::vector<int> min_edge_num     = { 0,  0,  0,  0,  0};
    std::vector<int> special_vertices = { 5,  5, 20, 40, 60};
    std::vector<int> max_res          = { 5,  8, 15, 25, 30};

    run_test_case(vertices_num, min_edge_num, special_vertices, max_res, clique_tag());
}

BOOST_AUTO_TEST_SUITE_END()
