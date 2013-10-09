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
typedef std::set<Edge> ResultTree;

void checkResult(Graph & g, std::set<Edge> & tree,
                 Cost & costs, Bound & degBounds,
                 int verticesNum, double bestCost) {
    int treeEdges(tree.size());
    double treeCost = std::accumulate(tree.begin(), tree.end(), 0., 
                        [&](double cost, Edge e){return cost + costs[e];});

    LOGLN("tree edges: " << treeEdges);
    BOOST_CHECK(treeEdges == verticesNum - 1);
    BOOST_CHECK(treeCost <= bestCost);
    
    auto verts = vertices(g);
    int numOfViolations(0);
    
    for (const Vertex & v : boost::make_iterator_range(verts.first, verts.second)) {
        int treeDeg(0);
        auto adjVertices = adjacent_vertices(v, g);
        
        for(const Vertex & u : boost::make_iterator_range(adjVertices.first, adjVertices.second)) {
            bool b; Edge e;
            std::tie(e, b) = boost::edge(v, u, g);
            assert(b);
            
            if (tree.count(e)) {
                ++treeDeg;
            }
        }
        
        BOOST_CHECK(treeDeg <= degBounds[v] + 1);
        if (treeDeg > degBounds[v]) {
            ++numOfViolations;
        }
    }
        
    LOGLN("Found cost = " << treeCost << ", cost upper bound = " << bestCost);
    LOGLN("Number of violated constraints = " << numOfViolations);
        
    Graph treeG(verticesNum);
        
    for (auto const & e : tree) {
        add_edge(source(e, g), target(e, g), treeG);
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

        LOGLN(fname);
        std::ifstream ifs(testDir + "/cases/" + fname + ".lgf");
        
        Graph g(verticesNum);
        Cost costs      = boost::get(boost::edge_weight, g);
        Bound degBounds = boost::get(boost::vertex_degree, g);
        Index indices   = boost::get(boost::vertex_index, g);
        
        paal::readBDMST(ifs, verticesNum, edgesNum, g, costs, degBounds, indices, bestCost);

        namespace ir = paal::ir;
        // default heuristics
        {
            typedef ir::BDMSTIRComponents<Graph> Comps;
            ResultTree tree;
            auto oracle(ir::make_BoundedDegreeMSTOracle(g));
            
            Comps comps(ir::make_RowGenerationSolveLP(oracle));

            ir::bounded_degree_mst_iterative_rounding(g, costs, degBounds, tree, std::move(comps));
    
            checkResult(g, tree, costs, degBounds, verticesNum, bestCost);
        }
        
        
        // non-default heuristics
        {
            typedef ir::BoundedDegreeMSTOracleComponents<ir::FindMostViolated> OracleComps;
            typedef ir::BoundedDegreeMSTOracle < Graph, OracleComps> Oracle;
            typedef ir::BDMSTIRComponents<Graph, ir::RowGenerationSolveLP <Oracle >> Comps;
            ResultTree tree2;
            Oracle oracle(g);
            Comps comps(ir::make_RowGenerationSolveLP(oracle));
            ir::bounded_degree_mst_iterative_rounding(g, costs, degBounds, tree2, std::move(comps));
    
            checkResult(g, tree2, costs, degBounds, verticesNum, bestCost);
        }

        //TODO check 3rd strategy
    }
}
