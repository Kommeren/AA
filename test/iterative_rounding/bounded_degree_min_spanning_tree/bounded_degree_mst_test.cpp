/**
 * @file bounded_degree_mst.cpp
 * @brief 
 * @author Piotr Godlewski
 * @version 1.0
 * @date 2013-06-04
 */

#define BOOST_TEST_MODULE bounded_degree_mst

#include <boost/test/unit_test.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/range/counting_range.hpp>

#include "utils/logger.hpp"
#include "paal/iterative_rounding/bounded_degree_min_spanning_tree/bounded_degree_mst.hpp"

using namespace  paal;
using namespace  paal::ir;

struct LogVisitor : public TrivialVisitor {
    template <typename Solution, typename LP>
    void solveLPToExtremePoint(const Solution &, LP & lp) {
        LOGLN(lp);
    }

    template <typename Solution, typename LP>
    void roundCol(const Solution &, LP & lp, ColId col, double val) {
        LOGLN("Column "<< col.get() << " rounded to " << val);
    }
    
    template <typename Solution, typename LP>
    void relaxRow(const Solution &, LP & lp, RowId row) {
        LOGLN("Relax row " << row.get());
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


BOOST_AUTO_TEST_CASE(bounded_degree_mst_test) {
    //sample problem
    LOGLN("Sample problem:");
    Graph g;
    Cost costs = get(boost::edge_weight, g);
    
    std::set<Edge> correctBdmst;
    
    typedef boost::graph_traits<Graph>::edge_descriptor Edge;
    typedef std::set<Edge> ResultTree;
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
    
    Bound degBounds = get(boost::vertex_degree, g);
    Index indices = get(boost::vertex_index, g);
    
    degBounds[0] = 1;
    degBounds[1] = 3;
    degBounds[2] = 2;
    degBounds[3] = 2;
    degBounds[4] = 1;
    degBounds[5] = 1;
    
    typedef BDMSTIRComponents<Graph> Components;
    auto oracle(make_BoundedDegreeMSTOracle(g));
    Components components(make_RowGenerationSolveLP(oracle));
    bounded_degree_mst_iterative_rounding(g, costs, degBounds, resultTree, std::move(components), LogVisitor());
    
    for (auto const & e : resultTree) {
        LOGLN("Edge (" << indices[source(e, g)] << ", " << indices[target(e, g)]
              << ") " << "in tree");
    }

    BOOST_CHECK_EQUAL(correctBdmst.size(),resultTree.size());
    BOOST_CHECK(std::equal(correctBdmst.begin(), correctBdmst.end(), resultTree.begin()));
}

BOOST_AUTO_TEST_CASE(bounded_degree_mst_invalid_test) {
    // invalid problem (disconnected graph)
    LOGLN("Invalid problem (disconnected graph):");
    Graph g;
    Cost costs = get(boost::edge_weight, g);

    typedef boost::graph_traits<Graph>::edge_descriptor Edge;
    typedef std::set<Edge> ResultTree;
    ResultTree resultTree;

    addEdge(g, costs, 0, 1, 1);
    addEdge(g, costs, 0, 2, 16);
    addEdge(g, costs, 0, 3, 76);
    addEdge(g, costs, 1, 2, 90);
    addEdge(g, costs, 1, 3, 37);
    addEdge(g, costs, 4, 5, 20);
    addEdge(g, costs, 4, 6, 15);
    addEdge(g, costs, 5, 6, 4);

    Bound degBounds = get(boost::vertex_degree, g);

    for (int i : boost::counting_range(0,6)) {
        degBounds[i] = 6;
    }

    typedef BDMSTIRComponents<Graph> Components;
    auto oracle(make_BoundedDegreeMSTOracle(g));
    Components components(make_RowGenerationSolveLP(oracle));
    auto bdmst(make_BoundedDegreeMST(g, costs, degBounds, resultTree));
    auto invalid = bdmst.checkInputValidity();

    BOOST_CHECK(invalid);
    LOGLN(*invalid);
}

