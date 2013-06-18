/**
 * @file cycle_cancellation.hpp
 * @brief contains various algorithms for min cost max flow. This is in the boost namespace because something like this is on the boost to do list and should be provided.
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-03-18
 */
#ifndef MIN_COST_MAX_FLOW_HPP
#define MIN_COST_MAX_FLOW_HPP 

#include <numeric>

#include <boost/property_map/property_map.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/graph_concepts.hpp>
#include <boost/pending/indirect_cmp.hpp>
#include <boost/pending/relaxed_heap.hpp>
#include <boost/graph/edmonds_karp_max_flow.hpp>
#include <boost/graph/bellman_ford_shortest_paths.hpp>
#include "vertices_to_edges.hpp"
#include "find_flow_cost.hpp"

namespace boost {


namespace detail {

template <class Graph, class DistanceMap, class ParentMap, class Weight>
    typename graph_traits<Graph>::vertex_descriptor
find_cycle_start(Graph & g, DistanceMap distance, ParentMap  pred, Weight weight) {
    typedef graph_traits<Graph> GTraits;
    typedef typename GTraits::vertex_descriptor VD;
    typedef typename GTraits::edge_iterator EI;
    
    int N = num_vertices(g);
    VD v = -1;
    EI i, end;
    for (boost::tie(i, end) = edges(g); i != end; ++i) {
        if (get(distance, source(*i, g)) + get(weight, *i) < 
             get(distance, target(*i,g))) {
             v = source(*i, g);
             break;
        }
    }
    assert(v != VD(-1));
    for(int i = 0; i < N + 1; ++i) {
        v = get(pred, v);
    }

    return v;
}

} //detail


template <class Graph, class Pred, class Distance, class Reversed, class ResidualCapacity, class Weight>
void cycle_cancellation(Graph &g, Weight weight, Reversed rev, ResidualCapacity residual_capacity, Pred pred, Distance distance) {
    typedef typename property_traits<Weight>::value_type Dist;

    typedef filtered_graph<Graph, is_residual_edge<ResidualCapacity> > ResGraph;
    ResGraph gres = detail::residual_graph(g, residual_capacity);
    
    typedef graph_traits<ResGraph> ResGTraits;
    typedef graph_traits<Graph> GTraits;
    typedef typename ResGTraits::edge_descriptor ED;
    typedef typename ResGTraits::vertex_descriptor VD;
    typedef typename ResGTraits::edge_iterator REI;
    typedef typename GTraits::edge_iterator EI;
    typedef typename GTraits::vertex_iterator VI;
    
    unsigned N = num_vertices(g);
    
    VI u_iter, u_end;
    for (tie(u_iter, u_end) = vertices(g); u_iter != u_end; ++u_iter) {
        put(pred, *u_iter, *u_iter);
        put(distance, *u_iter, 0);
    }

    while(!bellman_ford_shortest_paths(gres, N, 
        weight_map(weight).distance_map(distance).predecessor_map(pred))) {
        REI i, end;
        for (tie(i, end) = edges(gres); i != end; ++i) {
            if (get(distance, source(*i, gres)) + get(weight, *i) < 
                 get(distance, target(*i,gres))) {
                 put(pred, target(*i, gres), source(*i, gres));
                 put(distance, target(*i, gres), get(distance, source(*i, gres)) + get(weight, *i));
            }
        }

        VD cycleStart = detail::find_cycle_start(gres, distance, pred, weight);

        assert(get(pred, cycleStart) != cycleStart);
        detail::augment(g, cycleStart, cycleStart, detail::make_mapVerticesToEdges(pred, g), residual_capacity, rev);
        
        for (tie(u_iter, u_end) = vertices(g); u_iter != u_end; ++u_iter) {
            put(pred, *u_iter, *u_iter);
            put(distance, *u_iter, 0);
        }
    }
}

namespace detail {

template <class Graph, class ResidualCapacity, class Weight, class Reversed, class Pred, class Distance>
void cycle_cancellation_dispatch2(
        Graph &g, 
        Weight weight,
        Reversed rev,
        ResidualCapacity residual_capacity,
        Pred pred,
        Distance dist) {
    cycle_cancellation(g, weight, rev, residual_capacity, pred, dist);
}

//setting default distance map
template <class Graph, class Pred, class ResidualCapacity, class Weight, class Reversed>
void cycle_cancellation_dispatch2(
        Graph &g, 
        Weight weight,
        Reversed rev,
        ResidualCapacity residual_capacity,
        Pred pred,
        param_not_found) {
    typedef typename property_traits<Weight>::value_type D;

    std::vector<D> d_map(num_vertices(g));

    cycle_cancellation(g, weight, rev, residual_capacity, pred, &d_map[0]);
}

template <class Graph, class P, class T, class R, class ResidualCapacity, class Weight, class Reversed, class Pred>
void cycle_cancellation_dispatch1(
        Graph &g, 
        Weight weight, 
        Reversed rev,
        ResidualCapacity residual_capacity,
        Pred pred,
        const bgl_named_params<P, T, R>& params) {
    cycle_cancellation_dispatch2(g, weight, rev,residual_capacity,  pred, 
                                get_param(params, vertex_distance));
}

//setting default predecessors map
template <class Graph, class P, class T, class R, class ResidualCapacity, class Weight, class Reversed>
void cycle_cancellation_dispatch1(
        Graph &g, 
        Weight weight, 
        Reversed rev,
        ResidualCapacity residual_capacity,
        param_not_found,
        const bgl_named_params<P, T, R>& params) {
    typedef  typename graph_traits<Graph>::vertex_descriptor VD; 
    std::vector<VD> p_map(num_vertices(g)); 

    cycle_cancellation_dispatch2(g, weight, rev, residual_capacity, &p_map[0], 
                                get_param(params, vertex_distance)); 
}

}//detail

template <class Graph, class  P, class T, class R>
void cycle_cancellation(Graph &g,
        const bgl_named_params<P, T, R>& params) {
    cycle_cancellation_dispatch1(g, 
           choose_const_pmap(get_param(params, edge_weight), g, edge_weight),
           choose_const_pmap(get_param(params, edge_reverse), g, edge_reverse),
           choose_pmap(get_param(params, edge_residual_capacity), 
                       g, edge_residual_capacity),
           get_param(params, vertex_predecessor), 
           params);
}

template <class Graph>
void cycle_cancellation(Graph &g) {
    bgl_named_params<int, buffer_param_t> params(0);
    cycle_cancellation(g, params);
}


}

#endif /* MIN_COST_MAX_FLOW_HPP */
