/**
 * @file steiner_network_long_test.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-07-10
 */

#include <iterator>
#include <iostream>
#include <fstream>
#include <iomanip>

#include <boost/test/unit_test.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>

#include "paal/iterative_rounding/steiner_network/steiner_network.hpp"

#include "utils/logger.hpp"
#include "utils/read_bounded_deg_mst.hpp"
#include "utils/parse_file.hpp"

typedef boost::adjacency_list < boost::vecS, boost::vecS, boost::undirectedS,
                        boost::property < boost::vertex_degree_t, int,
                            boost::property < boost::vertex_index_t, int >
                                 >,
                        boost::property < boost::edge_weight_t, double > > Graph;
typedef boost::adjacency_list_traits < boost::vecS, boost::vecS, boost::undirectedS > Traits;
typedef boost::graph_traits < Graph >::edge_descriptor Edge;
typedef boost::graph_traits < Graph >::vertex_descriptor Vertex;

typedef boost::property_map < Graph, boost::vertex_degree_t >::type Bound;
typedef boost::property_map < Graph, boost::vertex_index_t >::type Index;
typedef boost::property_map < Graph, boost::edge_weight_t >::type Cost;

int restrictions(int i, int j) {
    return 2;
}

template <typename FindViolated = paal::ir::FindRandViolated, typename Restrictions>
void runTest(const Graph & g, const Cost & costs, const Restrictions & restrictions) {
    namespace ir = paal::ir;
    namespace lp = paal::lp;

    typedef std::vector<Edge> ResultNetwork;
    typedef ir::SteinerNetworkOracleComponents<FindViolated> OracleComponents;
    typedef ir::SteinerNetworkOracle<Graph, Restrictions, OracleComponents> Oracle;
    typedef ir::SteinerNetworkIRComponents<Graph, Restrictions, ResultNetwork,
        lp::RowGenerationSolveLP<Oracle>> Components;

    ResultNetwork resultNetwork;
    Oracle oracle(g, restrictions);
    Components components(lp::make_RowGenerationSolveLP(oracle));
    auto steinerNetwork(ir::make_SteinerNetwork(g, restrictions, costs,
                                        std::back_inserter(resultNetwork)));
    auto invalid = steinerNetwork.checkInputValidity();
    BOOST_CHECK(!invalid);

    auto probType = ir::solve_iterative_rounding(steinerNetwork,
                        std::move(components));
    BOOST_CHECK(probType == lp::OPTIMAL);
}

BOOST_AUTO_TEST_CASE(steiner_network_long) {
    std::string testDir = "test/data/BOUNDED_DEGREE_MST/";
    paal::parse("test/data/STEINER_NETWORK/cases.txt", [&](const std::string & fname, std::istream & is_test_cases) {
        int verticesNum, edgesNum;
        double bestCost;

        is_test_cases >> verticesNum >> edgesNum;

        LOGLN(fname);
        std::ifstream ifs(testDir + "/cases/" + fname + ".lgf");

        Graph g(verticesNum);
        Cost costs      = get(boost::edge_weight, g);
        Bound degBounds = get(boost::vertex_degree, g);
        Index indices   = get(boost::vertex_index, g);

        paal::readBDMST(ifs, verticesNum, edgesNum, g, costs, degBounds, indices, bestCost);

        // default heuristics
        for (int i : boost::irange(0, 5)) {
            LOGLN("random violated, seed " << i);
            srand(i);
            runTest(g, costs, restrictions);
        }

        // non-default heuristics
        if (verticesNum <= 80) {
            LOGLN("most violated");
            runTest<paal::ir::FindMostViolated>(g, costs, restrictions);
        }

        // non-default heuristics (slow, only for smaller instances)
        if (verticesNum < 50) {
            LOGLN("any violated");
            runTest<paal::ir::FindAnyViolated>(g, costs, restrictions);
        }
    });
}
