/**
 * @file steiner_tree_greedy.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-11-27
 */
#ifndef STEINER_TREE_GREEDY_HPP
#define STEINER_TREE_GREEDY_HPP

#include <boost/property_map/property_map.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/graph_concepts.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/named_function_params.hpp>
#include <boost/graph/two_bit_color_map.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/adjacency_matrix.hpp>
#include <boost/graph/prim_minimum_spanning_tree.hpp>
#include <boost/range/numeric.hpp>
#include <boost/range/algorithm/unique.hpp>
#include <boost/range/algorithm/fill.hpp>
#include <boost/range/algorithm/sort.hpp>
#include <boost/range/algorithm/copy.hpp>

#include <algorithm>

namespace paal {

/**
 * @brief enum indicates if given color represents terminal or NONTERMINAL.
 */
enum Terminals {
    NONTERMINAL,
    TERMINAL
};

/**
 * @brief non-named version of  steiner_tree_greedy
 *
 * @tparam Graph
 * @tparam OutputIterator
 * @tparam EdgeWeightMap
 * @tparam ColorMap
 * @param g - given graph
 * @param out - edge output iterator
 * @param edge_weight
 * @param color_map
 */
template <typename Graph, typename OutputIterator, typename EdgeWeightMap,
          typename ColorMap>
void steiner_tree_greedy(const Graph &g, OutputIterator out,
                         EdgeWeightMap edge_weight, ColorMap color_map) {
    using Value = typename boost::property_traits<EdgeWeightMap>::value_type;
    using TerminalGraph = boost::adjacency_matrix<
        boost::undirectedS, boost::no_property,
        boost::property<boost::edge_weight_t, Value>>;
    using Vertex = typename boost::graph_traits<Graph>::vertex_descriptor;
    using Edge = typename boost::graph_traits<Graph>::edge_descriptor;
    auto N = num_vertices(g);

    // distance array used in the dijkstra runs
    std::vector<int> distance(N);

    // computing terminals
    std::vector<int> terminals;
    auto terminals_nr =
        boost::accumulate(vertices(g), 0, [ = ](int sum, Vertex v) {
        return sum + get(color_map, v);
    });
    terminals.reserve(terminals_nr);
    for (auto v : boost::make_iterator_range(vertices(g))) {
        if (get(color_map, v) == Terminals::TERMINAL) {
            terminals.push_back(v);
        }
    }

    if (terminals.empty()) {
        return;
    }

    // computing distances between terminals
    // creating terminal_graph
    TerminalGraph terminal_graph(N);
    for (auto v_iter = terminals.begin(); v_iter != terminals.end(); ++v_iter) {
        boost::dijkstra_shortest_paths(g, *v_iter,
                                       boost::distance_map(&distance[0]));
        for (auto w :
             boost::make_iterator_range(std::next(v_iter), terminals.end())) {
            add_edge(*v_iter, w, distance[w], terminal_graph);
        }
    }

    // computing spanning tree on terminal_graph
    std::vector<int> terminals_predecessors(N);
    boost::prim_minimum_spanning_tree(terminal_graph, &terminals_predecessors[0],
                                      boost::root_vertex(terminals.front()));

    // computing result
    std::vector<Edge> tree_edges;
    tree_edges.reserve(terminals_nr);
    std::vector<Edge> vpred(N);
    for (auto v : terminals) {
        auto global_pred = terminals_predecessors[v];
        if (global_pred != v) {
            boost::fill(vpred, Edge());
            boost::dijkstra_shortest_paths(
                g, global_pred,
                boost::visitor(make_dijkstra_visitor(record_edge_predecessors(
                    &vpred[0], boost::on_edge_relaxed())))
                    .distance_map(&distance[0]).weight_map(edge_weight));
            auto local_pred = vpred[v];
            while (local_pred != Edge()) {
                tree_edges.push_back(local_pred);
                v = source(local_pred, g);
                local_pred = vpred[v];
            }
            assert(local_pred == Edge());
        }
    }
    boost::sort(tree_edges);
    boost::copy(boost::unique(tree_edges), out);
}

/**
 * @brief named version of  steiner_tree_greedy
 *
 * @tparam Graph
 * @tparam OutputIterator
 * @tparam P
 * @tparam T
 * @tparam R
 * @param g - given graph
 * @param out - edge output iterator
 * @param params
 */
template <typename Graph, typename OutputIterator, typename P, typename T,
          typename R>
void steiner_tree_greedy(const Graph &g, OutputIterator out,
                         const boost::bgl_named_params<P, T, R> &params) {
    steiner_tree_greedy(
        g, out, choose_const_pmap(get_param(params, boost::edge_weight), g,
                                  boost::edge_weight),
        choose_const_pmap(get_param(params, boost::vertex_color), g,
                          boost::vertex_color));
}

/**
 * @brief version of  steiner_tree_greedy with all default parameters
 *
 * @tparam Graph
 * @tparam OutputIterator
 * @param g - given graph
 * @param out - edge output iterator
 */
template <typename Graph, typename OutputIterator>
void steiner_tree_greedy(const Graph &g, OutputIterator out) {
    steiner_tree_greedy(g, out, boost::no_named_parameters());
}

} // paal

#endif /* STEINER_TREE_GREEDY_HPP */
