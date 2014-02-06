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

struct LogVisitor : public TrivialVisitor {
    template <typename Problem, typename LP>
    void solveLPToExtremePoint(const Problem &, LP & lp) {
        LOGLN(lp);
    }

    template <typename Problem, typename LP>
    void roundCol(const Problem &, LP & lp, lp::ColId col, double val) {
        LOGLN("Column "<< col.get() << " rounded to " << val);
    }

    template <typename Problem, typename LP>
    void relaxRow(const Problem &, LP & lp, lp::RowId row) {
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

Edge addEdge(Graph & g, Cost & cost, int u, int v, double c) {
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

    correctBdmst.insert(addEdge(g, costs, 1, 0, 173));
    correctBdmst.insert(addEdge(g, costs, 4, 2, 176));
                        addEdge(g, costs, 2, 3, 176);
                        addEdge(g, costs, 4, 3, 190);
    correctBdmst.insert(addEdge(g, costs, 3, 1, 37));
                        addEdge(g, costs, 4, 1, 260);
    correctBdmst.insert(addEdge(g, costs, 5, 3, 105));
    correctBdmst.insert(addEdge(g, costs, 2, 1, 84));
                        addEdge(g, costs, 5, 4, 243);
                        addEdge(g, costs, 4, 0, 259);

    ON_LOG(Index indices = get(boost::vertex_index, g));

    std::vector<int> degBounds = {1, 3, 2, 2, 1, 1};
    auto bounds = paal::utils::make_ArrayToFunctor(degBounds);

    typedef BDMSTIRComponents<Graph> Components;
    BoundedDegreeMSTOracle<> oracle;
    Components components(lp::make_RowGenerationSolveLP(oracle),
                          lp::make_RowGenerationResolveLP(oracle));
    bounded_degree_mst_iterative_rounding(g, bounds,
                    std::inserter(resultTree, resultTree.begin()),
                    std::move(components), LogVisitor());

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

    ON_LOG(Index indices = get(boost::vertex_index, g));

    auto bounds = [&](int){return 2;};

    typedef BDMSTIRComponents<Graph> Components;
    {
        ResultTree resultTree;
        BoundedDegreeMSTOracle<> oracle;
        Components components(lp::make_RowGenerationSolveLP(oracle),
                              lp::make_RowGenerationResolveLP(oracle));

        bounded_degree_mst_iterative_rounding(g, bounds, boost::weight_map(cost),
                    std::inserter(resultTree, resultTree.begin()),
                    std::move(components), LogVisitor());

        BOOST_CHECK_EQUAL(correctBdmst.size(),resultTree.size());
        BOOST_CHECK(std::equal(correctBdmst.begin(), correctBdmst.end(), resultTree.begin()));
    }
    {
        ResultTree resultTree;
        BoundedDegreeMSTOracle<> oracle;
        Components components(lp::make_RowGenerationSolveLP(oracle),
                              lp::make_RowGenerationResolveLP(oracle));

        auto bdmst(make_BoundedDegreeMST(g, bounds, boost::weight_map(cost),
                    std::inserter(resultTree, resultTree.begin())));
        solve_iterative_rounding(bdmst, std::move(components), LogVisitor());

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

    addEdge(g, costs, 0, 1, 1);
    addEdge(g, costs, 0, 2, 16);
    addEdge(g, costs, 0, 3, 76);
    addEdge(g, costs, 1, 2, 90);
    addEdge(g, costs, 1, 3, 37);
    addEdge(g, costs, 4, 5, 20);
    addEdge(g, costs, 4, 6, 15);
    addEdge(g, costs, 5, 6, 4);

    auto bounds = [&](int){return 6;};

    typedef BDMSTIRComponents<Graph> Components;
    BoundedDegreeMSTOracle<> oracle;
    Components components(lp::make_RowGenerationSolveLP(oracle),
                          lp::make_RowGenerationResolveLP(oracle));
    auto bdmst(make_BoundedDegreeMST(g, bounds, std::back_inserter(resultTree)));
    auto invalid = bdmst.checkInputValidity();

    BOOST_CHECK(invalid);
    LOGLN(*invalid);
}

BOOST_AUTO_TEST_CASE(bounded_degree_mst_infeasible_test) {
    // infeasible problem
    LOGLN("Infeasible problem:");
    Graph g;
    auto costs = get(boost::edge_weight, g);

    std::vector<Edge> resultTree;

    addEdge(g, costs, 0, 3, 5);
    addEdge(g, costs, 3, 5, 15);
    addEdge(g, costs, 5, 4, 7);
    addEdge(g, costs, 5, 1, 2);
    addEdge(g, costs, 1, 2, 78);
    addEdge(g, costs, 5, 9, 56);
    addEdge(g, costs, 8, 9, 13);
    addEdge(g, costs, 9, 7, 6);
    addEdge(g, costs, 5, 6, 1);

    std::vector<int> degBounds = {1, 2, 1, 2, 1, 3, 1, 1, 1, 3};
    auto bounds = paal::utils::make_ArrayToFunctor(degBounds);

    typedef BDMSTIRComponents<Graph> Components;
    BoundedDegreeMSTOracle<> oracle;
    Components components(lp::make_RowGenerationSolveLP(oracle),
                          lp::make_RowGenerationResolveLP(oracle));
    auto bdmst(make_BoundedDegreeMST(g, bounds, std::back_inserter(resultTree)));
    auto invalid = bdmst.checkInputValidity();

    BOOST_CHECK(!invalid);

    auto result = solve_iterative_rounding(bdmst, std::move(components));

    BOOST_CHECK(result.first == lp::INFEASIBLE);
}
BOOST_AUTO_TEST_SUITE_END()

