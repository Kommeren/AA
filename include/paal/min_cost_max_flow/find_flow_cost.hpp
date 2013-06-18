/**
 * @file find_flow_cost.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-06-14
 */
#ifndef FIND_FLOW_COST_HPP
namespace boost {

template<class Graph, class Capacity, class ResidualCapacity, class Weight>
typename property_traits<typename property_map < Graph, edge_capacity_t >::type>::value_type
find_flow_cost(const Graph & g, Capacity capacity, ResidualCapacity residual_capacity, Weight weight)
{
    typename graph_traits<Graph>::edge_iterator ei, end;

    typedef typename property_traits<typename property_map<Graph, edge_weight_t>::const_type>::value_type Cost;
    Cost cost = 0;
    tie(ei, end) = edges(g);
    for(;ei != end; ++ei) {
        if((capacity[*ei]) > Cost(0)) {
            cost +=  (capacity[*ei] - residual_capacity[*ei]) * weight[*ei];
        } 
    }
    return cost;
}

template <class Graph, class P, class T, class R> 
typename property_traits<typename property_map < Graph, edge_capacity_t >::type>::value_type
find_flow_cost(const Graph & g,
               const bgl_named_params<P, T, R>& params) {
    return find_flow_cost(g,
           choose_const_pmap(get_param(params, edge_capacity), g, edge_capacity),
           choose_const_pmap(get_param(params, edge_residual_capacity), 
                       g, edge_residual_capacity),
           choose_const_pmap(get_param(params, edge_weight), g, edge_weight));
}

template <class Graph>
typename property_traits<typename property_map < Graph, edge_capacity_t >::type>::value_type
find_flow_cost(const Graph &g) {
    bgl_named_params<int, buffer_param_t> params(0);
    return find_flow_cost(g, params);
}


} //boost

#define FIND_FLOW_COST_HPP 
#endif /* FIND_FLOW_COST_HPP */
