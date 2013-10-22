/**
 * @file bounded_degree_mst_example.cpp
 * @brief 
 * @author Piotr Godlewski
 * @version 1.0
 * @date 2013-11-21
 */

#include <set>

#include <boost/graph/adjacency_list.hpp>

#include "paal/iterative_rounding/iterative_rounding.hpp"
#include "paal/iterative_rounding/bounded_degree_min_spanning_tree/bounded_degree_mst.hpp"

typedef boost::property<boost::vertex_degree_t, int> VertexProp;
typedef boost::property<boost::edge_weight_t, int> EdgeProp;

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS,
                            VertexProp, EdgeProp> Graph;

int main() {
//! [Bounded-Degree Minimum Spanning Tree Example]
    Graph g;
    typedef boost::graph_traits<Graph>::edge_descriptor Edge;

    // sample data
    add_vertex(VertexProp(3), g);
    add_vertex(VertexProp(2), g);
    add_vertex(VertexProp(2), g);
    add_vertex(VertexProp(2), g);
    add_vertex(VertexProp(2), g);
    add_vertex(VertexProp(2), g);

    bool b;
    b =  add_edge(0, 1, EdgeProp(1), g).second;
    b &= add_edge(0, 2, EdgeProp(2), g).second;
    b &= add_edge(1, 2, EdgeProp(1), g).second;
    b &= add_edge(1, 3, EdgeProp(2), g).second;
    b &= add_edge(1, 4, EdgeProp(1), g).second;
    b &= add_edge(1, 5, EdgeProp(1), g).second;
    b &= add_edge(5, 0, EdgeProp(1), g).second;
    b &= add_edge(3, 4, EdgeProp(5), g).second;
    assert(b);

    typedef std::set<Edge> ResultTree;
    ResultTree resultTree;
    auto cost = get(boost::edge_weight, g);
    auto degreeBound = get(boost::vertex_degree, g);

    // optional input validity checking
    auto bdmst = paal::ir::make_BoundedDegreeMST(g, cost, degreeBound, resultTree);
    auto error = bdmst.checkInputValidity();
    if (error) {
        std::cerr << "The input is not valid!" << std::endl;
        std::cerr << *error << std::endl;
        return -1;
    }

    // solve it
    auto oracle(paal::ir::make_BoundedDegreeMSTOracle(g));
    paal::ir::BDMSTIRComponents<Graph> components(paal::lp::make_RowGenerationSolveLP(oracle));
    auto resultType = paal::ir::bounded_degree_mst_iterative_rounding(g, cost, degreeBound,
                resultTree, std::move(components));

    // print result
    if (resultType == paal::lp::OPTIMAL) {
        std::cout << "Edges in the spanning tree" << std::endl;
        for (auto const & e : resultTree) {
            std::cout << "Edge " << e << std::endl;
        }
    }
    else {
        std::cout << "The instance is infeasible" << std::endl;
    }
    paal::lp::GLP::freeEnv();
//! [Bounded-Degree Minimum Spanning Tree Example]
    return 0;
}

