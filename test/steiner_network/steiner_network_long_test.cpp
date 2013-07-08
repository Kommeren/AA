/**
 * @file steiner_network_long_test.cpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-07-10
 */

#define BOOST_TEST_MODULE steiner_network_test

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

/*void checkResult(Graph & g, std::map<Edge, bool> & tree,
                 Cost & costs, Bound & degBounds,
                 int verticesNum, double bestCost) {
    int treeEdges(0);
    double treeCost(0);
    for (const std::pair<Edge, bool> & e : tree) {
        if (e.second) {
            ++treeEdges;
            treeCost += costs[e.first];
        }
    }

    LOG("tree edges: " << treeEdges);
    BOOST_CHECK(treeEdges == verticesNum - 1);
    BOOST_CHECK(treeCost <= bestCost);
    
    auto verts = vertices(g);
    int numOfViolations(0);
    
    for (const Vertex & v : paal::utils::make_range(verts.first, verts.second)) {
        int treeDeg(0);
        auto adjVertices = adjacent_vertices(v, g);
        
        for(const Vertex & u : paal::utils::make_range(adjVertices.first, adjVertices.second)) {
            bool b; Edge e;
            std::tie(e, b) = boost::edge(v, u, g);
            assert(b);
            
            if (tree[e]) {
                ++treeDeg;
            }
        }
        
        BOOST_CHECK(treeDeg <= degBounds[v] + 1);
        if (treeDeg > degBounds[v]) {
            ++numOfViolations;
        }
    }
        
    LOG("Found cost = " << treeCost << ", cost upper bound = " << bestCost);
    LOG("Number of violated constraints = " << numOfViolations);
        
    Graph treeG(verticesNum);
        
    for (const std::pair<Edge, bool> & e : tree) {
        if (e.second) {
            add_edge(source(e.first, g), target(e.first, g), treeG);
        }
    }
        
    std::vector<int> component(verticesNum);
    BOOST_CHECK(connected_components(treeG, &component[0]) == 1);
}*/

int restrictions(int i, int j) {
    return 2;
}

BOOST_AUTO_TEST_CASE(bounded_degree_mst_long) {
    std::string testDir = "test/data/BOUNDED_DEGREE_MST/";
    std::ifstream is_test_cases("test/data/STEINER_NETWORK/cases.txt");

    assert(is_test_cases.good());
    while(is_test_cases.good()) {
        std::string fname;
        int MAX_LINE = 256;
        char buf[MAX_LINE];
        int verticesNum, edgesNum;
        double bestCost;
        is_test_cases.getline(buf, MAX_LINE);
        if(buf[0] == 0) {
            return;
        }
        
        if(buf[0] == '#')
            continue;
        std::stringstream ss;
        ss << buf;
        
        ss >> fname >> verticesNum >> edgesNum;

        LOG(fname);
        std::ifstream ifs(testDir + "/cases/" + fname + ".lgf");
        
        Graph g(verticesNum);
        Cost costs      = boost::get(boost::edge_weight, g);
        Bound degBounds = boost::get(boost::vertex_degree, g);
        Index indices   = boost::get(boost::vertex_index, g);
        
        paal::readBDMST(ifs, verticesNum, edgesNum, g, costs, degBounds, indices, bestCost);

        // default heuristics
        auto ga = paal::ir::make_SteinerNetwork(g, costs, restrictions);
        paal::ir::IterativeRounding<decltype(ga)> ir(std::move(ga));
        paal::ir::solve(ir);
    
        auto & args = ir.getSolution();
     //   checkResult(g, tree, costs, degBounds, verticesNum, bestCost);
    }
}
