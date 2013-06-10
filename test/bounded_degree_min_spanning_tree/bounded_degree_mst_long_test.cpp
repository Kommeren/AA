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
#include <boost/range/irange.hpp>
#include <boost/graph/adjacency_list.hpp>

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

typedef property_map < Graph, vertex_degree_t >::type Bound;
typedef property_map < Graph, vertex_index_t >::type Index;
typedef property_map < Graph, edge_weight_t >::type Cost;

BOOST_AUTO_TEST_CASE(bounded_degree_mst_long) {
    std::string testDir = "test/data/BOUNDED_DEGREE_MST/";
    std::ifstream is_test_cases(testDir + "bdmst.txt");

    assert(is_test_cases.good());
    while(is_test_cases.good()) {
        std::string fname;
        int MAX_LINE = 256;
        char buf[MAX_LINE];
        int verticesNum, edgesNum;
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
        
        paal::readBDMST(ifs, verticesNum, edgesNum, g, costs, degBounds, indices);

        auto ga = paal::ir::make_BoundedDegreeMST(g, costs, degBounds);
        paal::ir::IterativeRounding<decltype(ga)> ir(std::move(ga));
        paal::ir::solve(ir);
    
        ON_LOG(auto & s = ir.getEngine().getSolution(ir.getLP()));
    }
}
