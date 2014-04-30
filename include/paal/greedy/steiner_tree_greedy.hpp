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
 * @param edgeWeight
 * @param colorMap
 */
template <typename Graph, typename OutputIterator, typename EdgeWeightMap, typename ColorMap>
void steiner_tree_greedy(const Graph & g, OutputIterator out, EdgeWeightMap edgeWeight, ColorMap colorMap) {
    typedef typename boost::property_traits<EdgeWeightMap>::value_type value;
    typedef boost::adjacency_matrix<boost::undirectedS, boost::no_property,
                boost::property<boost::edge_weight_t, value>> TerminalGraph;
    typedef typename boost::graph_traits<Graph>::vertex_descriptor vertex;
    typedef typename boost::graph_traits<Graph>::edge_descriptor edge;
    auto N = num_vertices(g);

    //distance array used in the dijkstra runs
    std::vector<int> distance(N);

    //computing terminals
    std::vector<int> terminals;
    auto terminalsNr = boost::accumulate(vertices(g), 0,
            [=](int sum, vertex v){return sum + get(colorMap, v);});
    terminals.reserve(terminalsNr);
    for(auto v: boost::make_iterator_range(vertices(g))) {
        if(get(colorMap, v) == Terminals::TERMINAL) {
            terminals.push_back(v);
        }
    }

    if(terminals.empty()) {
        return;
    }

    //computing distances between terminals
    //creating terminalGraph
    TerminalGraph terminalGraph(N);
    for(auto vIter = terminals.begin(); vIter != terminals.end(); ++vIter) {
        boost::dijkstra_shortest_paths(g, *vIter, boost::distance_map(&distance[0]));
        for(auto w : boost::make_iterator_range(std::next(vIter), terminals.end())) {
            add_edge(*vIter, w, distance[w], terminalGraph);
        }
    }

    //computing spanning tree on terminalGraph
    std::vector<int> terminalsPredecessors(N);
    boost::prim_minimum_spanning_tree(terminalGraph,
            &terminalsPredecessors[0],
            boost::root_vertex(terminals.front()));

    //computing result
    std::vector<edge> treeEdges;
    treeEdges.reserve(terminalsNr);
    std::vector<edge> vpred(N);
    for(auto v : terminals) {
        auto globalPred = terminalsPredecessors[v];
        if(globalPred != v) {
            boost::fill(vpred, edge());
            boost::dijkstra_shortest_paths(g, globalPred,
                boost::visitor(make_dijkstra_visitor(record_edge_predecessors(&vpred[0], boost::on_edge_relaxed()))).
                    distance_map(&distance[0]).
                    weight_map(edgeWeight));
            auto localPred = vpred[v];
            while(localPred != edge()) {
                treeEdges.push_back(localPred);
                v = source(localPred, g);
                localPred = vpred[v];
            }
            assert(localPred == edge());
        }
    }
    boost::sort(treeEdges);
    boost::copy(boost::unique(treeEdges), out);
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
template <typename Graph, typename OutputIterator, typename P, typename T, typename R>
void steiner_tree_greedy(const Graph &g, OutputIterator out,
        const boost::bgl_named_params<P, T, R>& params) {
    steiner_tree_greedy(g, out,
           choose_const_pmap(get_param(params, boost::edge_weight), g, boost::edge_weight),
           choose_const_pmap(get_param(params, boost::vertex_color), g, boost::vertex_color));
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


} //paal

#endif /* STEINER_TREE_GREEDY_HPP */
