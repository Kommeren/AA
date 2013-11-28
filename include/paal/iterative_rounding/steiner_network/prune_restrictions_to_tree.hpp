/**
 * @file prune_restrictions_to_tree.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-07-05
 */
#ifndef PRUNE_RESTRICTIONS_TO_TREE_HPP
#define PRUNE_RESTRICTIONS_TO_TREE_HPP 

#include <boost/range/irange.hpp>
#include <boost/function_output_iterator.hpp>

namespace paal {

typedef std::vector<std::pair<unsigned, unsigned>> RestrictionsVector;

template <typename G>
    struct EdgeToVertexPairsIterator {
        typedef typename boost::graph_traits<G>::edge_descriptor Edge;

        EdgeToVertexPairsIterator(const G & g, RestrictionsVector & resVec) :
            m_restrictionsVec(&resVec), m_g(&g) {}

        void operator()(Edge e) {
            m_restrictionsVec->push_back(std::make_pair(source(e, *m_g), target(e, *m_g)));
        }

        RestrictionsVector * m_restrictionsVec;
        const G * m_g;
    };


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
    RestrictionsVector pruneRestrictionsToTree(Restrictions res, int N) {
        typedef decltype(std::declval<Restrictions>()(0,0)) Dist;
        typedef boost::property < boost::edge_weight_t, Dist> EdgeProp;
        typedef boost::adjacency_list < boost::vecS, boost::vecS, boost::undirectedS,
                boost::no_property, EdgeProp> TGraph;

        RestrictionsVector resVec;
        TGraph g(N);
        for (int i : boost::irange(0, N)) {
            for (int j : boost::irange(i + 1, N)) {
                add_edge(i, j, 
                        EdgeProp(-std::max(res(i, j), res(j, i))),  g);
            }
        }
        
        EdgeToVertexPairsIterator<TGraph> addEdge(g, resVec);

        boost::kruskal_minimum_spanning_tree(g, 
                boost::make_function_output_iterator(addEdge));
        return resVec;
    }

}

#endif /* PRUNE_RESTRICTIONS_TO_TREE_HPP */
