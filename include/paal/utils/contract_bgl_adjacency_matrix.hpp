/**
 * @file contract_bgl_adjacency_matrix.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-22
 */
#ifndef CONTRACT_BGL_ADJACENCY_MATRIX_HPP
#define CONTRACT_BGL_ADJACENCY_MATRIX_HPP

#include <boost/graph/adjacency_matrix.hpp>
#include "paal/utils/type_functions.hpp"

namespace paal {
namespace utils {

//template <typename Idx, typename... Args>
//On gcc 4.6 I get sorry unimplemented ... !!!

template <typename Directed, 
          typename VertexProperty, 
          typename EdgeProperty, 
          typename GraphProperty,
          typename Allocator,
          typename Idx>
void contract(
boost::adjacency_matrix<Directed, 
                        VertexProperty, 
                        EdgeProperty, 
                        GraphProperty,
                        Allocator > &amatrix, Idx v, Idx w) {
    typedef boost::graph_traits<puretype(amatrix)> mtraits;
    typedef typename mtraits::edge_descriptor MEdge;
    auto const & weight_map = boost::get(boost::edge_weight, amatrix);
    weight_map[boost::edge(v, w, amatrix).first] = 0;
    for(const MEdge & e : boost::make_iterator_range(boost::out_edges(v, amatrix))) {
        MEdge  f = boost::edge(w, boost::target(e, amatrix), amatrix).first;
        auto & we = weight_map[e];
        auto & wf = weight_map[f];
        we = std::min(we, wf);
        wf = we;

        // TODO hide  checkking
        auto  teste = boost::edge(boost::target(e, amatrix), w, amatrix).first;
        auto  testf = boost::edge(boost::target(e, amatrix), v, amatrix).first;
        auto wte = weight_map[teste];
        auto wtf = weight_map[testf];
        assert(wte == wtf && wte == we);
    }
}


}
}
#endif /* CONTRACT_BGL_ADJACENCY_MATRIX_HPP */
