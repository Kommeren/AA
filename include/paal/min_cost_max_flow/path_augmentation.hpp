/**
 * @file path_augmentation.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-06-14
 */
#ifndef PATH_AUGMENTATION_HPP
#define PATH_AUGMENTATION_HPP 

#include <numeric>

#include <boost/property_map/property_map.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/graph_concepts.hpp>
#include <boost/pending/indirect_cmp.hpp>
#include <boost/pending/relaxed_heap.hpp>
#include <boost/graph/edmonds_karp_max_flow.hpp>
#include <boost/graph/bellman_ford_shortest_paths.hpp>

#include "vertices_to_edges.hpp"

namespace boost {


template <class Graph, class Capacity, class ResidualCapacity, class Reversed, class Pred, class Weight, class Distance>
void path_augmentation(
        Graph &g, 
        typename graph_traits<Graph>::vertex_descriptor s, 
        typename graph_traits<Graph>::vertex_descriptor t,
        Capacity capacity,
        ResidualCapacity residual_capacity,
        Weight weight, 
        Reversed rev,
        Pred pred, 
        Distance distance) {
    typedef typename graph_traits < Graph>::edge_descriptor ED;
    typedef typename graph_traits < Graph>::vertex_descriptor VD;
    typedef typename graph_traits < Graph>::edge_iterator EI;
    typedef typename graph_traits < Graph>::vertex_iterator VI;
    typedef typename property_traits<Weight>::value_type Dist;

    unsigned N = num_vertices(g);

    filtered_graph<Graph, is_residual_edge<ResidualCapacity> >
        gres = detail::residual_graph(g, residual_capacity);
    
    VI u_iter, u_end;
    EI ei, e_end;
    for (tie(ei, e_end) = edges(g); ei != e_end; ++ei) {
        put(residual_capacity, *ei, get(capacity, *ei));
    }

    while(true) {
        for (tie(u_iter, u_end) = vertices(g); u_iter != u_end; ++u_iter) {
            put(pred, *u_iter, *u_iter);
            put(distance, *u_iter, std::numeric_limits<Dist>::max());
        }
        put(distance, s, 0);
        bool b =  bellman_ford_shortest_paths(gres, int(N), 
            weight_map(weight).distance_map(distance).predecessor_map(pred));

        assert(b);
        if(VD(get(pred, t)) == t) {
            break;
        }

        detail::augment(g, s, t, detail::make_mapVerticesToEdges(pred, g), residual_capacity, rev);
    }
}

namespace detail {

template <class Graph, class Capacity, class ResidualCapacity, class Weight, class Reversed, class Pred, class Distance>
void path_augmentation_dispatch2(
        Graph &g, 
        typename graph_traits<Graph>::vertex_descriptor s, 
        typename graph_traits<Graph>::vertex_descriptor t,
        Capacity capacity,
        ResidualCapacity residual_capacity,
        Weight weight,
        Reversed rev,
        Pred pred,
        Distance dist) {
    path_augmentation(g, s, t, capacity, residual_capacity, weight, rev, pred, dist);
}

//setting default distance map
template <class Graph, class Capacity, class ResidualCapacity, class Weight, class Reversed, class Pred>
void path_augmentation_dispatch2(
        Graph &g, 
        typename graph_traits<Graph>::vertex_descriptor s, 
        typename graph_traits<Graph>::vertex_descriptor t,
        Capacity capacity,
        ResidualCapacity residual_capacity,
        Weight weight,
        Reversed rev,
        Pred pred,
        param_not_found) {
    typedef typename property_traits<Weight>::value_type D;

    std::vector<D> d_map(num_vertices(g));

    path_augmentation(g, s, t, capacity, residual_capacity, weight, rev, pred, &d_map[0]);
}

template <class Graph, class P, class T, class R, class Capacity, class ResidualCapacity, class Weight, class Reversed, class Pred>
void path_augmentation_dispatch1(
        Graph &g, 
        typename graph_traits<Graph>::vertex_descriptor s, 
        typename graph_traits<Graph>::vertex_descriptor t,
        Capacity capacity,
        ResidualCapacity residual_capacity,
        Weight weight, 
        Reversed rev,
        Pred pred,
        const bgl_named_params<P, T, R>& params) {
    path_augmentation_dispatch2(g, s, t, capacity, residual_capacity, weight,  rev, pred,
                                get_param(params, vertex_distance));
}

//setting default predecessors map
template <class Graph, class P, class T, class R, class Capacity, class ResidualCapacity, class Weight, class Reversed>
void path_augmentation_dispatch1(
        Graph &g, 
        typename graph_traits<Graph>::vertex_descriptor s, 
        typename graph_traits<Graph>::vertex_descriptor t,
        Capacity capacity,
        ResidualCapacity residual_capacity,
        Weight weight, 
        Reversed rev,
        param_not_found,
        const bgl_named_params<P, T, R>& params) {
    typedef  typename graph_traits<Graph>::vertex_descriptor VD; 
    std::vector<VD> p_map(num_vertices(g)); 

    path_augmentation_dispatch2(g, s, t, capacity, residual_capacity, weight, rev, &p_map[0], 
                                get_param(params, vertex_distance)); 
}

}//detail


template <class Graph, class P, class T, class R>
void path_augmentation(
        Graph &g, 
        typename graph_traits<Graph>::vertex_descriptor s, 
        typename graph_traits<Graph>::vertex_descriptor t,
        const bgl_named_params<P, T, R>& params) {
           
    return detail::path_augmentation_dispatch1(g, s, t, 
           choose_const_pmap(get_param(params, edge_capacity), g, edge_capacity),
           choose_pmap(get_param(params, edge_residual_capacity), 
                       g, edge_residual_capacity),
           choose_const_pmap(get_param(params, edge_weight), g, edge_weight),
           choose_const_pmap(get_param(params, edge_reverse), g, edge_reverse),
           get_param(params, vertex_predecessor), 
           params);
}

template <class Graph>
void path_augmentation(
        Graph &g,
        typename graph_traits<Graph>::vertex_descriptor s, 
        typename graph_traits<Graph>::vertex_descriptor t) {
    bgl_named_params<int, buffer_param_t> params(0);
    path_augmentation(g, s, t, params);
}


}
#endif /* PATH_AUGMENTATION_HPP */
