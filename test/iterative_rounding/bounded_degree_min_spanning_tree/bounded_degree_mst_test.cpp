/**
 * @file bounded_degree_mst.cpp
 * @brief
 * @author Piotr Godlewski
 * @version 1.0
 * @date 2013-06-04
 */

#include <boost/test/unit_test.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/range/counting_range.hpp>
#include <boost/property_map/property_map.hpp>

#include "utils/logger.hpp"
#include "paal/utils/functors.hpp"
#include "paal/iterative_rounding/bounded_degree_min_spanning_tree/bounded_degree_mst.hpp"

using namespace  paal;
using namespace  paal::ir;

struct log_visitor : public trivial_visitor {
    template <typename Problem, typename LP>
    void solve_lp_to_extreme_point(const Problem &, LP & lp) {
        LOGLN(lp);
    }

    template <typename Problem, typename LP>
    void round_col(const Problem &, LP & lp, lp::col_id col, double val) {
        LOGLN("Column "<< col.get() << " rounded to " << val);
    }

    template <typename Problem, typename LP>
    void relax_row(const Problem &, LP & lp, lp::row_id row) {
        LOGLN("Relax row " << row.get());
    }
};

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS,
                            boost::property<boost::vertex_index_t, int>,
                            boost::property<boost::edge_index_t, std::size_t,
                                boost::property<boost::edge_weight_t, double>>> Graph;
typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::undirectedS> Traits;
typedef boost::graph_traits<Graph>::edge_descriptor Edge;

typedef boost::property_map<Graph, boost::edge_weight_t>::type Cost;
typedef boost::property_map<Graph, boost::vertex_index_t>::type Index;

typedef std::set<Edge> ResultTree;

Edge add_edge_to_graph(Graph & g, Cost & cost, int u, int v, double c) {
    bool b;
    Traits::edge_descriptor e;
    std::tie(e, b) = add_edge(u, v, g);
    assert(b);
    cost[e] = c;
    return e;
}

BOOST_AUTO_TEST_SUITE(bounded_degree_mst)
BOOST_AUTO_TEST_CASE(bounded_degree_mst_test) {
    //sample problem
    LOGLN("Sample problem:");
    Graph g;
    auto costs = get(boost::edge_weight, g);

    ResultTree correctBdmst;
    ResultTree resultTree;

    correctBdmst.insert(add_edge_to_graph(g, costs, 1, 0, 173));
    correctBdmst.insert(add_edge_to_graph(g, costs, 4, 2, 176));
                        add_edge_to_graph(g, costs, 2, 3, 176);
                        add_edge_to_graph(g, costs, 4, 3, 190);
    correctBdmst.insert(add_edge_to_graph(g, costs, 3, 1, 37));
                        add_edge_to_graph(g, costs, 4, 1, 260);
    correctBdmst.insert(add_edge_to_graph(g, costs, 5, 3, 105));
    correctBdmst.insert(add_edge_to_graph(g, costs, 2, 1, 84));
                        add_edge_to_graph(g, costs, 5, 4, 243);
                        add_edge_to_graph(g, costs, 4, 0, 259);

    ON_LOG(Index indices = get(boost::vertex_index, g));

    std::vector<int> degBounds = {1, 3, 2, 2, 1, 1};
    auto bounds = paal::utils::make_array_to_functor(degBounds);

    bounded_degree_mst_iterative_rounding(g, bounds,
                    std::inserter(resultTree, resultTree.begin()),
                    BDMSTIRcomponents<>(), BDMSTOracle<>(),
                    log_visitor());

    ON_LOG(for (auto const & e : resultTree) {
        LOGLN("Edge (" << indices[source(e, g)] << ", " << indices[target(e, g)]
              << ") " << "in tree");
    })

    BOOST_CHECK_EQUAL(correctBdmst.size(),resultTree.size());
    BOOST_CHECK(std::equal(correctBdmst.begin(), correctBdmst.end(), resultTree.begin()));
}

BOOST_AUTO_TEST_CASE(bounded_degree_mst_test_parameters) {
    //sample problem
    LOGLN("Sample problem:");
    Graph g;

    ResultTree correctBdmst;

    correctBdmst.insert(add_edge(1, 0, 0, g).first);
                        add_edge(2, 1, 1, g);
    correctBdmst.insert(add_edge(0, 2, 2, g).first);

    std::vector<double> costs = {173, 176, 37};
    auto cost = boost::make_iterator_property_map(costs.begin(), get(boost::edge_index, g));

    auto bounds = [&](int){return 2;};
    {
        ResultTree resultTree;
        bounded_degree_mst_iterative_rounding(g, bounds, boost::weight_map(cost),
                    std::inserter(resultTree, resultTree.begin()),
                    BDMSTIRcomponents<>(), BDMSTOracle<>(),
                    log_visitor());

        BOOST_CHECK_EQUAL(correctBdmst.size(),resultTree.size());
        BOOST_CHECK(std::equal(correctBdmst.begin(), correctBdmst.end(), resultTree.begin()));
    }
    {
        ResultTree resultTree;
        auto bdmst(make_bounded_degree_mst(g, bounds, boost::weight_map(cost),
                    std::inserter(resultTree, resultTree.begin())));
        solve_iterative_rounding(bdmst, BDMSTIRcomponents<>(), log_visitor());

        BOOST_CHECK_EQUAL(correctBdmst.size(),resultTree.size());
        BOOST_CHECK(std::equal(correctBdmst.begin(), correctBdmst.end(), resultTree.begin()));
    }
}

BOOST_AUTO_TEST_CASE(bounded_degree_mst_invalid_test) {
    // invalid problem (disconnected graph)
    LOGLN("Invalid problem (disconnected graph):");
    Graph g;
    auto costs = get(boost::edge_weight, g);

    std::vector<Edge> resultTree;

    add_edge_to_graph(g, costs, 0, 1, 1);
    add_edge_to_graph(g, costs, 0, 2, 16);
    add_edge_to_graph(g, costs, 0, 3, 76);
    add_edge_to_graph(g, costs, 1, 2, 90);
    add_edge_to_graph(g, costs, 1, 3, 37);
    add_edge_to_graph(g, costs, 4, 5, 20);
    add_edge_to_graph(g, costs, 4, 6, 15);
    add_edge_to_graph(g, costs, 5, 6, 4);

    auto bounds = [&](int){return 6;};

    auto bdmst(make_bounded_degree_mst(g, bounds, std::back_inserter(resultTree)));
    auto invalid = bdmst.check_input_validity();

    BOOST_CHECK(invalid);
    LOGLN(*invalid);
}

BOOST_AUTO_TEST_CASE(bounded_degree_mst_infeasible_test) {
    // infeasible problem
    LOGLN("Infeasible problem:");
    Graph g;
    auto costs = get(boost::edge_weight, g);

    std::vector<Edge> resultTree;

    add_edge_to_graph(g, costs, 0, 3, 5);
    add_edge_to_graph(g, costs, 3, 5, 15);
    add_edge_to_graph(g, costs, 5, 4, 7);
    add_edge_to_graph(g, costs, 5, 1, 2);
    add_edge_to_graph(g, costs, 1, 2, 78);
    add_edge_to_graph(g, costs, 5, 9, 56);
    add_edge_to_graph(g, costs, 8, 9, 13);
    add_edge_to_graph(g, costs, 9, 7, 6);
    add_edge_to_graph(g, costs, 5, 6, 1);

    std::vector<int> degBounds = {1, 2, 1, 2, 1, 3, 1, 1, 1, 3};
    auto bounds = paal::utils::make_array_to_functor(degBounds);

    auto bdmst(make_bounded_degree_mst(g, bounds, std::back_inserter(resultTree)));
    auto invalid = bdmst.check_input_validity();

    BOOST_CHECK(!invalid);

    auto result = solve_iterative_rounding(bdmst, BDMSTIRcomponents<>());

    BOOST_CHECK(result.first == lp::INFEASIBLE);
}
BOOST_AUTO_TEST_SUITE_END()

