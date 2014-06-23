/**
 * @file steiner_network_test.cpp
 * @brief
 * @author Piotr Wygocki, Piotr Godlewski
 * @version 1.0
 * @date 2013-06-24
 */

#include "utils/logger.hpp"

#include "paal/iterative_rounding/iterative_rounding.hpp"
#include "paal/iterative_rounding/steiner_network/steiner_network.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/test/unit_test.hpp>

#include <vector>

using namespace paal;
using namespace paal::ir;

template <typename VertexList, typename EdgeProp>
using Graph = boost::adjacency_list<boost::vecS, VertexList, boost::undirectedS,
                                    boost::property<boost::vertex_index_t, int>,
                                    EdgeProp>;

using EdgeProp = boost::property<boost::edge_index_t, std::size_t,
                                 boost::property<boost::edge_weight_t, int>>;
using VectorGraph = Graph<boost::vecS, EdgeProp>;

using Edge = boost::graph_traits<VectorGraph>::edge_descriptor;
using ResultNetwork = std::vector<Edge>;

int restrictions(int i, int j) { return 2; }

void print_result(const ResultNetwork & result_network) {
    LOGLN("Edges in steiner network");
    ON_LOG(for(auto const e : result_network) {
        LOGLN("Edge " << e);
    })
}

BOOST_AUTO_TEST_SUITE(steiner_network)
BOOST_AUTO_TEST_CASE(steiner_network_test) {
    // sample problem
    LOGLN("Sample problem:");
    VectorGraph g(3);
    ResultNetwork result_network;
    bool b;
    b = add_edge(0, 1, EdgeProp(0, 1), g).second;
    b &= add_edge(0, 1, EdgeProp(1, 1), g).second;
    b &= add_edge(1, 2, EdgeProp(2, 1), g).second;
    b &= add_edge(1, 2, EdgeProp(3, 1), g).second;
    b &= add_edge(2, 0, EdgeProp(4, 7), g).second;
    assert(b);

    //solve it
    steiner_network_iterative_rounding(
        g, restrictions, std::back_inserter(result_network));

    print_result(result_network);
    BOOST_CHECK_EQUAL(result_network.size(), 4);
}

BOOST_AUTO_TEST_CASE(steiner_network_list) {
    // boost::listS instead of boost::vecS for vertex storage
    using ListGraph =
        Graph<boost::listS, boost::property<boost::edge_weight_t, int>>;
    using EdgeT = boost::graph_traits<ListGraph>::edge_descriptor;

    std::vector<std::pair<int, int>> edges = { { 0, 1 }, { 0, 1 }, { 1, 2 },
                                               { 1, 2 }, { 2, 0 } };
    std::vector<int> costs{ 1, 1, 1, 1, 7 };
    ListGraph g(edges.begin(), edges.end(), costs.begin(), 3);

    auto index = get(boost::vertex_index, g);
    int idx = 0;
    for (auto v : boost::make_iterator_range(vertices(g))) {
        put(index, v, idx);
        ++idx;
    }

    std::vector<EdgeT> result_network;
    steiner_network_iterative_rounding(
        g, restrictions, std::back_inserter(result_network));
    BOOST_CHECK_EQUAL(result_network.size(), 4);
}

BOOST_AUTO_TEST_CASE(steiner_network_test_properties) {
    // sample problem
    LOGLN("Sample problem:");
    VectorGraph g(3);

    bool b;
    b = add_edge(0, 1, 0, g).second;
    b &= add_edge(0, 1, 1, g).second;
    b &= add_edge(1, 2, 2, g).second;
    b &= add_edge(1, 2, 3, g).second;
    b &= add_edge(2, 0, 4, g).second;
    assert(b);

    auto edge_id = get(boost::edge_index, g);
    std::vector<double> costs = {1, 1, 1, 1, 7};
    auto cost = boost::make_iterator_property_map(costs.begin(), edge_id);

    // solve it
    {
        ResultNetwork result_network;
        steiner_network_iterative_rounding(g, restrictions,
            boost::weight_map(cost), std::back_inserter(result_network));

        print_result(result_network);
    }
    {
        ResultNetwork result_network;
        auto steiner_network(make_steiner_network(g, restrictions,
                    boost::weight_map(cost),
                    std::back_inserter(result_network)));
        solve_iterative_rounding(steiner_network, steiner_network_ir_components<>());

        print_result(result_network);
    }
}

BOOST_AUTO_TEST_CASE(steiner_network_invalid_test) {
    // invalid problem (restrictions cannot be satisfied)
    LOGLN("Invalid problem (restrictions cannot be satisfied):");
    VectorGraph g(3);
    ResultNetwork result_network;
    bool b;
    b = add_edge(0, 1, EdgeProp(0, 1), g).second;
    b &= add_edge(1, 2, EdgeProp(1, 1), g).second;
    assert(b);

    //solve it
    auto steiner_network(make_steiner_network(g, restrictions,
                                    std::back_inserter(result_network)));
    auto invalid = steiner_network.check_input_validity();

    BOOST_CHECK(invalid);
    LOGLN(*invalid);
}
BOOST_AUTO_TEST_SUITE_END()
