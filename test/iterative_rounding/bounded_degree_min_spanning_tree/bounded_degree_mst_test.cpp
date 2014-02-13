//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file bounded_degree_mst.cpp
 * @brief
 * @author Piotr Godlewski
 * @version 1.0
 * @date 2013-06-04
 */

#include "test_utils/logger.hpp"
#include "iterative_rounding/log_visitor.hpp"

#include "paal/iterative_rounding/bounded_degree_min_spanning_tree/bounded_degree_mst.hpp"
#include "paal/utils/functors.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/range/counting_range.hpp>
#include <boost/test/unit_test.hpp>


using namespace paal;
using namespace paal::ir;


template <typename VertexList, typename EdgeProp>
using Graph = boost::adjacency_list<boost::vecS, VertexList, boost::undirectedS,
                                    boost::property<boost::vertex_index_t, int>,
                                    EdgeProp>;

using EdgeProp = boost::property<boost::edge_index_t, std::size_t,
                                 boost::property<boost::edge_weight_t, int>>;
using VectorGraph = Graph<boost::vecS, EdgeProp>;

using Edge = boost::graph_traits<VectorGraph>::edge_descriptor;
using ResultTree = std::set<Edge>;

template <typename Cost>
Edge add_edge_to_graph(VectorGraph &g, Cost &cost, int u, int v, double c) {
    bool b;
    Edge e;
    std::tie(e, b) = add_edge(u, v, g);
    assert(b);
    cost[e] = c;
    return e;
}

BOOST_AUTO_TEST_SUITE(bounded_degree_mst)
BOOST_AUTO_TEST_CASE(bounded_degree_mst_test) {
    // sample problem
    LOGLN("Sample problem:");
    VectorGraph g;
    auto costs = get(boost::edge_weight, g);

    ResultTree correct_bdmst;
    ResultTree result_tree;

    correct_bdmst.insert(add_edge_to_graph(g, costs, 1, 0, 173));
    correct_bdmst.insert(add_edge_to_graph(g, costs, 4, 2, 176));
                         add_edge_to_graph(g, costs, 2, 3, 176);
                         add_edge_to_graph(g, costs, 4, 3, 190);
    correct_bdmst.insert(add_edge_to_graph(g, costs, 3, 1, 37));
                         add_edge_to_graph(g, costs, 4, 1, 260);
    correct_bdmst.insert(add_edge_to_graph(g, costs, 5, 3, 105));
    correct_bdmst.insert(add_edge_to_graph(g, costs, 2, 1, 84));
                         add_edge_to_graph(g, costs, 5, 4, 243);
                         add_edge_to_graph(g, costs, 4, 0, 259);

    ON_LOG(auto indices = get(boost::vertex_index, g));

    std::vector<int> deg_bounds = {1, 3, 2, 2, 1, 1};
    auto bounds = paal::utils::make_array_to_functor(deg_bounds);

    bounded_degree_mst_iterative_rounding(g, bounds,
                    std::inserter(result_tree, result_tree.begin()),
                    bdmst_ir_components<>{}, lp::random_violated_separation_oracle{},
                    log_visitor{});

    ON_LOG(for (auto const & e : result_tree) {
        LOGLN("Edge (" << indices[source(e, g)] << ", " << indices[target(e, g)]
                       << ") "
                       << "in tree");
    })

    BOOST_CHECK_EQUAL(correct_bdmst.size(),result_tree.size());
    BOOST_CHECK(std::equal(correct_bdmst.begin(), correct_bdmst.end(), result_tree.begin()));
}

BOOST_AUTO_TEST_CASE(bounded_degree_mst_test_parameters) {
    // sample problem
    LOGLN("Sample problem:");
    VectorGraph g;

    ResultTree correct_bdmst;

    correct_bdmst.insert(add_edge(1, 0, 0, g).first);
                         add_edge(2, 1, 1, g);
    correct_bdmst.insert(add_edge(0, 2, 2, g).first);

    std::vector<double> costs = { 173, 176, 37 };
    auto cost = boost::make_iterator_property_map(costs.begin(),
                                                  get(boost::edge_index, g));

    auto bounds = [&](int) { return 2; };

    {
        ResultTree result_tree;
        bounded_degree_mst_iterative_rounding(g, bounds, boost::weight_map(cost),
                    std::inserter(result_tree, result_tree.begin()),
                    bdmst_ir_components<>{}, paal::lp::random_violated_separation_oracle{},
                    log_visitor{});

        BOOST_CHECK_EQUAL(correct_bdmst.size(),result_tree.size());
        BOOST_CHECK(std::equal(correct_bdmst.begin(), correct_bdmst.end(), result_tree.begin()));
    }
    {
        ResultTree result_tree;
        auto bdmst(make_bounded_degree_mst(g, bounds, boost::weight_map(cost),
                    std::inserter(result_tree, result_tree.begin())));
        solve_iterative_rounding(bdmst, bdmst_ir_components<>{}, log_visitor{});

        BOOST_CHECK_EQUAL(correct_bdmst.size(),result_tree.size());
        BOOST_CHECK(std::equal(correct_bdmst.begin(), correct_bdmst.end(), result_tree.begin()));
    }
}

BOOST_AUTO_TEST_CASE(bounded_degree_mst_list) {
    // boost::listS instead of boost::vecS for vertex storage
    using ListGraph =
        Graph<boost::listS, boost::property<boost::edge_weight_t, double>>;
    using EdgeT = boost::graph_traits<ListGraph>::edge_descriptor;

    std::vector<std::pair<int, int>> edges = { { 1, 0 }, { 4, 2 }, { 2, 3 },
                                               { 4, 3 }, { 3, 1 }, { 4, 1 },
                                               { 5, 3 }, { 2, 1 }, { 5, 4 },
                                               { 4, 0 } };
    std::vector<double> costs = { 173, 176, 176, 190, 37, 260, 105, 84, 243,
                                  259 };
    ListGraph g(edges.begin(), edges.end(), costs.begin(), 6);

    auto index = get(boost::vertex_index, g);
    int idx = 0;
    for (auto v : boost::make_iterator_range(vertices(g))) {
        put(index, v, idx);
        ++idx;
    }

    std::vector<int> deg_bounds = {1, 3, 2, 2, 1, 1};
    auto bounds = paal::utils::make_array_to_functor(deg_bounds);

    std::vector<EdgeT> result_tree;
    bounded_degree_mst_iterative_rounding(g, bounds,
                    std::inserter(result_tree, result_tree.begin()));
    BOOST_CHECK_EQUAL(result_tree.size(), 5);
}

BOOST_AUTO_TEST_CASE(bounded_degree_mst_invalid_test) {
    // invalid problem (disconnected graph)
    LOGLN("Invalid problem (disconnected graph):");
    VectorGraph g;
    auto costs = get(boost::edge_weight, g);

    std::vector<Edge> result_tree;

    add_edge_to_graph(g, costs, 0, 1, 1);
    add_edge_to_graph(g, costs, 0, 2, 16);
    add_edge_to_graph(g, costs, 0, 3, 76);
    add_edge_to_graph(g, costs, 1, 2, 90);
    add_edge_to_graph(g, costs, 1, 3, 37);
    add_edge_to_graph(g, costs, 4, 5, 20);
    add_edge_to_graph(g, costs, 4, 6, 15);
    add_edge_to_graph(g, costs, 5, 6, 4);

    auto bounds = [&](int) { return 6; };

    auto bdmst(make_bounded_degree_mst(g, bounds, std::back_inserter(result_tree)));
    auto invalid = bdmst.check_input_validity();

    BOOST_CHECK(invalid);
    LOGLN(*invalid);
}

BOOST_AUTO_TEST_CASE(bounded_degree_mst_infeasible_test) {
    // infeasible problem
    LOGLN("Infeasible problem:");
    VectorGraph g;
    auto costs = get(boost::edge_weight, g);

    std::vector<Edge> result_tree;

    add_edge_to_graph(g, costs, 0, 3, 5);
    add_edge_to_graph(g, costs, 3, 5, 15);
    add_edge_to_graph(g, costs, 5, 4, 7);
    add_edge_to_graph(g, costs, 5, 1, 2);
    add_edge_to_graph(g, costs, 1, 2, 78);
    add_edge_to_graph(g, costs, 5, 9, 56);
    add_edge_to_graph(g, costs, 8, 9, 13);
    add_edge_to_graph(g, costs, 9, 7, 6);
    add_edge_to_graph(g, costs, 5, 6, 1);

    std::vector<int> deg_bounds = {1, 2, 1, 2, 1, 3, 1, 1, 1, 3};
    auto bounds = paal::utils::make_array_to_functor(deg_bounds);

    auto bdmst(make_bounded_degree_mst(g, bounds, std::back_inserter(result_tree)));
    auto invalid = bdmst.check_input_validity();

    BOOST_CHECK(!invalid);

    auto result = solve_iterative_rounding(bdmst, bdmst_ir_components<>{});

    BOOST_CHECK(result.first == lp::INFEASIBLE);
}
BOOST_AUTO_TEST_SUITE_END()
