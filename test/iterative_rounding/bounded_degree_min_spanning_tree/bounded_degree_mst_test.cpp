/**
 * @file bounded_degree_mst.cpp
 * @brief 
 * @author Piotr Godlewski
 * @version 1.0
 * @date 2013-06-04
 */

#define BOOST_TEST_MODULE bounded_degree_mst_test

#include <boost/test/unit_test.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "utils/logger.hpp"
#include "paal/iterative_rounding/bounded_degree_min_spanning_tree/bounded_degree_mst.hpp"

using namespace  paal;
using namespace  paal::ir;

struct LogVisitor : public TrivialVisitor {

    template <typename LP>
    void roundCol(LP & lp, ColId col, double val) {
        LOG("Column "<< col.get() << " rounded to " << val);
    }
    
    template <typename LP>
    void relaxRow(LP & lp, RowId row) {
        LOG("Relax row " << row.get());
    }
};

typedef boost::adjacency_list < boost::vecS, boost::vecS, boost::undirectedS,
                            boost::property < boost::vertex_degree_t, int,
                                boost::property < boost::vertex_index_t, int >
                                     >,
                            boost::property < boost::edge_weight_t, double > > Graph;
typedef boost::adjacency_list_traits < boost::vecS, boost::vecS, boost::undirectedS > Traits;
typedef boost::graph_traits < Graph >::edge_descriptor Edge;

template <typename Graph, typename Cost>
Edge addEdge(Graph & g, Cost & cost, int u, int v, double c) {
    bool b;
    Traits::edge_descriptor e;
    std::tie(e, b) = add_edge(u, v, g);
    assert(b);
    cost[e] = c;
    return e;
}

typedef boost::property_map < Graph, boost::vertex_degree_t >::type Bound;
typedef boost::property_map < Graph, boost::vertex_index_t >::type Index;
typedef boost::property_map < Graph, boost::edge_weight_t >::type Cost;

BOOST_AUTO_TEST_CASE(bounded_degree_mst) {
    //sample problem
    Graph g;
    Cost costs = boost::get(boost::edge_weight, g);
    
    std::map<Edge, bool> correctBdmst;
   
    correctBdmst[addEdge(g, costs, 1, 0, 173)] = true;
    correctBdmst[addEdge(g, costs, 4, 2, 176)] = true;
    correctBdmst[addEdge(g, costs, 2, 3, 176)] = false;
    correctBdmst[addEdge(g, costs, 4, 3, 190)] = false;
    correctBdmst[addEdge(g, costs, 3, 1, 37)] = true;
    correctBdmst[addEdge(g, costs, 4, 1, 260)] = false;
    correctBdmst[addEdge(g, costs, 5, 3, 105)] = true;
    correctBdmst[addEdge(g, costs, 2, 1, 84)] = true;
    correctBdmst[addEdge(g, costs, 5, 4, 243)] = false;
    correctBdmst[addEdge(g, costs, 4, 0, 259)] = false;
    
    Bound degBounds = boost::get(boost::vertex_degree, g);
    Index indices = boost::get(boost::vertex_index, g);
    
    degBounds[0] = 1;
    degBounds[1] = 3;
    degBounds[2] = 2;
    degBounds[3] = 2;
    degBounds[4] = 1;
    degBounds[5] = 1;
    
    auto bdmst = make_BoundedDegreeMST(g, costs, degBounds);
    
    IterativeRounding<decltype(bdmst), LogVisitor> ir(std::move(bdmst));
    
    LOG(ir.solveLPToExtremePoint());
    ir.round();
    ir.relax();
    
    BOOST_CHECK(ir.integerSolution());
    
    auto const & tree = ir.getSolution();
    for (const std::pair<Edge, bool> & e : tree) {
        LOG("Edge (" << indices[source(e.first, g)] << ", " << indices[target(e.first, g)]
              << ") "<< (e.second ? "" : "not ") << "in tree");
        
        BOOST_CHECK(e.second == correctBdmst[e.first]);
    }
    
}

