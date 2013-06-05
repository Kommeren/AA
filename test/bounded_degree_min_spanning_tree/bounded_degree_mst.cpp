/**
 * @file bounded_degree_mst_test.cpp
 * @brief 
 * @author Piotr Godlewski
 * @version 1.0
 * @date 2013-06-04
 */

#define BOOST_TEST_MODULE bounded_degree_mst_test

#include <boost/test/unit_test.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "utils/logger.hpp"
#include "paal/iterative_rounding/iterative_rounding.hpp"
#include "paal/iterative_rounding/bounded_degree_min_spanning_tree/bounded_degree_mst.hpp"

using namespace  paal;
using namespace  paal::ir;
using namespace  boost;

struct LogVisitor : public TrivialVisitor {

    template <typename LP>
    void roundCol(LP & lp, int col, double val) {
        LOG("Column "<< col << " rounded to " << val);
    }
    
    template <typename LP>
    void relaxRow(LP & lp, int row) {
        LOG("Relax row " << row);
    }
};

typedef adjacency_list < vecS, vecS, undirectedS,
                            property < vertex_degree_t, int,
                                property < vertex_index_t, int >
                                     >,
                            property < edge_weight_t, double > > Graph;
typedef adjacency_list_traits < vecS, vecS, undirectedS > Traits;

template <typename Graph, typename Cost>
void addEdge(Graph & g, Cost & cost, int u, int v, double c) {
    bool b;
    Traits::edge_descriptor e;
    std::tie(e, b) = add_edge(u, v, g);
    assert(b);
    cost[e] = c;
}

typedef graph_traits < Graph >::edge_descriptor Edge;
typedef property_map < Graph, vertex_degree_t >::type Bound;
typedef property_map < Graph, vertex_index_t >::type Index;
typedef property_map < Graph, edge_weight_t >::type Cost;

BOOST_AUTO_TEST_CASE(bounded_degree_mst) {
    //sample problem
    Graph g;
    Cost costs = get(edge_weight, g);
   
    addEdge(g, costs, 1, 0, 173);
    addEdge(g, costs, 2, 1, 84);
    addEdge(g, costs, 3, 1, 37);
    addEdge(g, costs, 4, 2, 176);
    addEdge(g, costs, 2, 3, 176);
    addEdge(g, costs, 4, 3, 190);
    addEdge(g, costs, 4, 1, 260);
    addEdge(g, costs, 5, 3, 105);
    addEdge(g, costs, 5, 4, 243);
    addEdge(g, costs, 4, 0, 259);
    
    Bound degBounds = get(vertex_degree, g);
    Index indices = get(vertex_index, g);
    
    degBounds[0] = 1;
    degBounds[1] = 3;
    degBounds[2] = 2;
    degBounds[3] = 2;
    degBounds[4] = 1;
    degBounds[5] = 1;
    
    auto bdmst = make_BoundedDegreeMST(g, costs, degBounds);
    
    IterativeRounding<decltype(bdmst), LogVisitor> ir(std::move(bdmst));
    
    LOG(ir.solve());
    ir.round();
    ir.relax();
    
    BOOST_CHECK(ir.integerSolution());
    
    auto const & tree = ir.getSolution();
    for (const std::pair<Edge, bool> & e : tree) {
        LOG("Edge (" << indices[source(e.first, g)] << ", " << indices[target(e.first, g)]
              << ") "<< (e.second ? "" : "not ") << "in tree");
    }
    
}

