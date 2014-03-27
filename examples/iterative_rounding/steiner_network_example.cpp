/**
 * @file steiner_network_example.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-06-24
 */

#include <vector>

#include <boost/graph/adjacency_list.hpp>

#include "paal/iterative_rounding/iterative_rounding.hpp"
#include "paal/iterative_rounding/steiner_network/steiner_network.hpp"


//! [Steiner Network Example]

int main() {
    typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS,
        boost::no_property, boost::property<boost::edge_weight_t, int>> Graph;
    typedef boost::graph_traits<Graph>::edge_descriptor Edge;
    auto restrictions = [](int i, int j) {return 2;};

    // sample problem
    std::vector<std::pair<int, int>> edges {{0,1},{0,1},{1,2},{1,2},{2,0}};
    std::vector<int> costs {1,1,1,1,7};

    Graph g(edges.begin(), edges.end(), costs.begin(), 3);

    typedef std::vector<Edge> ResultNetwork;
    ResultNetwork resultNetwork;

    // optional input validity checking
    auto steinerNetwork = paal::ir::make_steiner_network(g, restrictions,
                                std::back_inserter(resultNetwork));
    auto error = steinerNetwork.check_input_validity();
    if (error) {
        std::cerr << "The input is not valid!" << std::endl;
        std::cerr << *error << std::endl;
        return -1;
    }

    // solve it
    auto result = paal::ir::steiner_network_iterative_rounding(
                    g, restrictions, std::back_inserter(resultNetwork));

    // print result
    if (result.first == paal::lp::OPTIMAL) {
        std::cout << "Edges in steiner network" << std::endl;
        for (auto const  & e : resultNetwork) {
            std::cout << "Edge " << e << std::endl;
        }
        std::cout << "Cost of the solution: " << *(result.second) << std::endl;
    }
    else {
        std::cout << "The instance is infeasible" << std::endl;
    }
    paal::lp::GLP::free_env();

    return 0;
}
//! [Steiner Network Example]

