/**
 * @file prune_restrictions_to_tree.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-07-05
 */
#ifndef PRUNE_RESTRICTIONS_TO_TREE_HPP
#define PRUNE_RESTRICTIONS_TO_TREE_HPP


#include "paal/utils/functors.hpp"

#include <boost/range/irange.hpp>
#include <boost/function_output_iterator.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>

namespace paal {

typedef std::vector<std::pair<unsigned, unsigned>> RestrictionsVector;

/**
 * @brief Returns a list of restrictions, made of the edges of a maximum spanning tree
 * in a clique with edge weights equal to restriction values between the edges.
 *
 * @tparam Restrictions
 * @param res restrictions
 * @param N number of vertices
 *
 * @return A minimum set of restrictions needed to be checked by the oracle.
 */
template <typename Restrictions>
    RestrictionsVector prune_restrictions_to_tree(Restrictions res, int N) {
        typedef decltype(std::declval<Restrictions>()(0,0)) Dist;
        typedef boost::property < boost::edge_weight_t, Dist> EdgeProp;
        typedef boost::adjacency_list < boost::vecS, boost::vecS, boost::undirectedS,
                boost::no_property, EdgeProp> TGraph;
        typedef typename boost::graph_traits<TGraph>::edge_descriptor Edge;

        RestrictionsVector resVec;
        TGraph g(N);
        for (int i : boost::irange(0, N)) {
            for (int j : boost::irange(i + 1, N)) {
                add_edge(i, j,
                        EdgeProp(-std::max(res(i, j), res(j, i))),  g);
            }
        }

        auto add_edge_to_graph = [&](Edge e){resVec.push_back(std::make_pair(source(e, g), target(e,g)));};
        boost::kruskal_minimum_spanning_tree(g,
                boost::make_function_output_iterator(utils::make_assignable_functor(add_edge_to_graph)));
        return resVec;
    }

}

#endif /* PRUNE_RESTRICTIONS_TO_TREE_HPP */
