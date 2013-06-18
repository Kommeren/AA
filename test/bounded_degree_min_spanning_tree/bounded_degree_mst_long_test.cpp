/**
 * @file bounded_degree_mst_long_test.cpp
 * @brief 
 * @author Piotr Godlewski
 * @version 1.0
 * @date 2013-06-10
 */

#define BOOST_TEST_MODULE bounded_degree_mst_long_test

#include <iterator>
#include <iostream>
#include <fstream>
#include <iomanip>

#include <boost/test/unit_test.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>

#include "utils/logger.hpp"
#include "utils/read_bounded_deg_mst.hpp"
#include "paal/iterative_rounding/iterative_rounding.hpp"
#include "paal/iterative_rounding/bounded_degree_min_spanning_tree/bounded_degree_mst.hpp"

using namespace boost;

typedef adjacency_list < vecS, vecS, undirectedS,
                        property < vertex_degree_t, int,
                            property < vertex_index_t, int >
                                 >,
                        property < edge_weight_t, double > > Graph;
typedef adjacency_list_traits < vecS, vecS, undirectedS > Traits;
typedef graph_traits < Graph >::edge_descriptor Edge;
typedef graph_traits < Graph >::vertex_descriptor Vertex;

typedef property_map < Graph, vertex_degree_t >::type Bound;
typedef property_map < Graph, vertex_index_t >::type Index;
typedef property_map < Graph, edge_weight_t >::type Cost;

void checkResult(Graph & g, std::map<Edge, bool> & tree,
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
}

BOOST_AUTO_TEST_CASE(bounded_degree_mst_long) {
    std::string testDir = "test/data/BOUNDED_DEGREE_MST/";
    std::ifstream is_test_cases(testDir + "bdmst.txt");

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
        Cost costs      = get(edge_weight, g);
        Bound degBounds = get(vertex_degree, g);
        Index indices   = get(vertex_index, g);
        
        paal::readBDMST(ifs, verticesNum, edgesNum, g, costs, degBounds, indices, bestCost);

        // default heuristics
        auto ga = paal::ir::make_BoundedDegreeMST(g, costs, degBounds);
        paal::ir::IterativeRounding<decltype(ga)> ir(std::move(ga));
        paal::ir::solve(ir);
    
        auto & tree = ir.getSolution();
        checkResult(g, tree, costs, degBounds, verticesNum, bestCost);
        
        // non-default heuristics
        auto ga2 = paal::ir::BoundedDegreeMST<Graph, Cost, Bound,
                                              paal::ir::BoundedDegreeMSTOracleComponents<paal::ir::FindMostViolated> >(g, costs, degBounds);
        paal::ir::IterativeRounding<decltype(ga2)> ir2(std::move(ga2));
        paal::ir::solve(ir2);
    
        auto & tree2 = ir2.getSolution();
        checkResult(g, tree2, costs, degBounds, verticesNum, bestCost);
    }
}
