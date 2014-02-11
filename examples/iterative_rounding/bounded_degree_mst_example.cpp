/**
 * @file bounded_degree_mst_example.cpp
 * @brief
 * @author Piotr Godlewski
 * @version 1.0
 * @date 2013-11-21
 */

#include <vector>

#include <boost/graph/adjacency_list.hpp>

#include "paal/utils/functors.hpp"
#include "paal/iterative_rounding/iterative_rounding.hpp"
#include "paal/iterative_rounding/bounded_degree_min_spanning_tree/bounded_degree_mst.hpp"

int main() {
//! [Bounded-Degree Minimum Spanning Tree Example]
    typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS,
        boost::no_property, boost::property<boost::edge_weight_t, int>> Graph;
    typedef boost::graph_traits<Graph>::edge_descriptor Edge;

    // sample problem
    std::vector<std::pair<int, int>> edges {{0,1},{0,2},{1,2},{1,3},{1,4},{1,5},{5,0},{3,4}};
    std::vector<int> costs {1,2,1,2,1,1,1,5};
    std::vector<int> bounds {3,2,2,2,2,2};

    Graph g(edges.begin(), edges.end(), costs.begin(), 6);
    auto degreeBounds = paal::utils::make_ArrayToFunctor(bounds);

    typedef std::vector<Edge> ResultTree;
    ResultTree resultTree;

    // optional input validity checking
    auto bdmst = paal::ir::make_BoundedDegreeMST(g, degreeBounds,
                                            std::back_inserter(resultTree));
    auto error = bdmst.checkInputValidity();
    if (error) {
        std::cerr << "The input is not valid!" << std::endl;
        std::cerr << *error << std::endl;
        return -1;
    }

    // solve it
    auto oracle(paal::ir::make_BoundedDegreeMSTOracle(g));
    paal::ir::BDMSTIRComponents<Graph> components(
            paal::lp::make_RowGenerationSolveLP(oracle),
            paal::lp::make_RowGenerationResolveLP(oracle));
    auto resultType = paal::ir::bounded_degree_mst_iterative_rounding(g, degreeBounds,
                            std::back_inserter(resultTree), std::move(components));

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

