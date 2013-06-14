/**
 * @file min_cost_max_flow.hpp
 * @brief contains various algorithms for min cost max flow. This is in the boost namespace because something like this is on the boost to do list and should be provided.
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-03-18
 */
#ifndef MIN_COST_MAX_FLOW_HPP
#define MIN_COST_MAX_FLOW_HPP 

#define BOOST_RESULT_OF_USE_DECLTYPE
#include <numeric>

#include <boost/property_map/property_map.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/graph_concepts.hpp>
#include <boost/pending/indirect_cmp.hpp>
#include <boost/pending/relaxed_heap.hpp>
#include <boost/graph/edmonds_karp_max_flow.hpp>
#include <boost/graph/bellman_ford_shortest_paths.hpp>
#include <boost/iterator/transform_iterator.hpp>

namespace boost {

template<class Graph>
typename property_traits<typename property_map < Graph, edge_capacity_t >::type>::value_type
find_min_cost(const Graph & g)
{
    typename graph_traits<Graph>::edge_iterator ei, end;
    auto residual_capacity = get(edge_residual_capacity, g);
    auto capacity = get(edge_capacity, g);
    auto weight = get(edge_weight, g);

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
namespace detail {

template <typename Pred, typename Graph>
class MapVerticesToEdges {
    typedef graph_traits<Graph> gtraits;
    typedef typename gtraits::edge_descriptor ED;
    typedef typename gtraits::vertex_descriptor VD;
public:
    MapVerticesToEdges(const Pred & p, const Graph & g) : 
        pred_(p), g_(g) {}

     ED operator()(VD v) const {
        bool b;
        ED e;
        tie(e ,b) = edge(pred_[v], v, g_);
//        assert(b);
        return e;
    }
private:
    const Pred & pred_;
    const Graph & g_;
};

template <typename Pred, typename Graph>
MapVerticesToEdges<Pred, Graph> 
make_mapVerticesToEdges(const Pred & p, const Graph & g)  {
    return MapVerticesToEdges<Pred, Graph>(p, g);
}

}//detail

template <typename Pred, typename Graph>
typename graph_traits<Graph>::edge_descriptor 
get(const detail::MapVerticesToEdges<Pred, Graph> & map, typename graph_traits<Graph>::vertex_descriptor v) {
    return map(v);
}

template <typename Graph>
void path_augmentation(Graph &gOrig, typename graph_traits<Graph>::vertex_descriptor s, 
                                                       typename graph_traits<Graph>::vertex_descriptor t) {
    typedef typename property_map < Graph, edge_weight_t >::type Weight;
    typedef typename graph_traits < Graph>::edge_descriptor ED;
    typedef typename graph_traits < Graph>::out_edge_iterator OEI;
    typedef typename graph_traits < Graph>::vertex_descriptor VD;
    typedef typename graph_traits < Graph>::vertex_iterator VI;
    typedef typename property_traits<Weight>::value_type Dist;
    typedef typename property_map < Graph, edge_residual_capacity_t >::type ResidualCapacity;
    typedef typename property_map < Graph, edge_reverse_t >::type Reversed;
    typedef std::vector<VD> Pred;

    ResidualCapacity residual_capacity = get(edge_residual_capacity, gOrig); 
    auto g = detail::residual_graph(gOrig, residual_capacity);

    unsigned N = num_vertices(g);
    Weight weight = get(edge_weight, g);
    std::vector<Dist> distance(N);
    Pred  pred(N);
    std::iota(pred.begin(), pred.end(), 0);
    auto capacity = get(edge_capacity, g);
    Reversed rev = get(edge_reverse, g);
    
    VI u_iter, u_end;
    OEI ei, e_end;
    for (tie(u_iter, u_end) = vertices(gOrig); u_iter != u_end; ++u_iter) {
        for (tie(ei, e_end) = out_edges(*u_iter, gOrig); ei != e_end; ++ei) {
            put(residual_capacity, *ei, get(capacity, *ei));
        }
    }

    while(true) {
        std::iota(pred.begin(), pred.end(), 0);
        std::fill(distance.begin(), distance.end(),(std::numeric_limits<Dist>::max)());
        distance[s] = 0;
        bool b =  bellman_ford_shortest_paths(g, int(N), 
            weight_map(weight).distance_map(&distance[0]).predecessor_map(&pred[0]));

        assert(b);
        if(pred[t] == t) {
            break;
        }

        detail::augment(gOrig, s, t, detail::make_mapVerticesToEdges(pred, gOrig), residual_capacity, rev);
    }
}

template <typename Graph, typename DistanceMap, typename ParentMap>
    typename graph_traits<Graph>::vertex_descriptor
find_cycle_start(Graph & g, const DistanceMap & distance, const ParentMap & pred) {
    typedef graph_traits<Graph> GTraits;
    typedef typename GTraits::vertex_descriptor VD;
    typedef typename GTraits::edge_iterator EI;
    typedef typename property_map <Graph, edge_weight_t >::type Weight;
    
    int N = num_vertices(g);
    Weight weight = get(edge_weight, g);
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
        v = pred[v];
    }

    return v;
}


template <typename Graph>
void cycle_cancellation(Graph &gOrig) {
    typedef typename property_map < Graph, edge_weight_t >::type Weight;
    typedef typename property_traits<Weight>::value_type Dist;

    auto residual_capacity = get(edge_residual_capacity, gOrig); 
    auto g = detail::residual_graph(gOrig, residual_capacity);
    
    typedef graph_traits<decltype(g)> ResGTraits;
    typedef graph_traits<Graph> GTraits;
    typedef typename ResGTraits::edge_descriptor ED;
    typedef typename ResGTraits::vertex_descriptor VD;
    typedef typename ResGTraits::edge_iterator REI;
    typedef typename GTraits::edge_iterator EI;
    
    unsigned N = num_vertices(g);
    typedef std::vector<VD> Pred;
    auto weight = get(edge_weight, g);
    std::vector<Dist> distance(N);
    Pred  pred(N);
    auto rev = get(edge_reverse, gOrig);
    
    std::iota(pred.begin(), pred.end(), 0);
    std::fill(distance.begin(), distance.end(), 0);
    while(!bellman_ford_shortest_paths(g, N, 
        weight_map(weight).distance_map(&distance[0]).predecessor_map(&pred[0]))) {
        REI i, end;
        for (tie(i, end) = edges(g); i != end; ++i) {
            if (get(&distance[0], source(*i, g)) + get(weight, *i) < 
                 get(&distance[0], target(*i,g))) {
                 pred[target(*i, g)] = source(*i, g);
                 distance[target(*i, g)] = get(&distance[0], source(*i, g)) + get(weight, *i);
            }
        }

        VD cycleStart = find_cycle_start(g, &distance[0], &pred[0]);

        assert(pred[cycleStart] != cycleStart);
        detail::augment(gOrig, cycleStart, cycleStart, detail::make_mapVerticesToEdges(pred, gOrig), residual_capacity, rev);
        
        std::iota(pred.begin(), pred.end(), 0);
        std::fill(distance.begin(), distance.end(),0);
    }
}
    
template <typename Graph> 
void edmonds_karp_without_init(Graph &g, 
        typename graph_traits<Graph>::vertex_descriptor source,
        typename graph_traits<Graph>::vertex_descriptor sink) {
    typedef typename graph_traits < Graph>::vertex_descriptor VD;
    typedef typename graph_traits < Graph>::edge_descriptor ED;
    unsigned N = num_vertices(g);
    std::vector<default_color_type> color(N);
    typedef typename property_traits<decltype(&color[0])>::value_type ColorValue;
    typedef color_traits<ColorValue> Color;
    std::vector<ED> pred(N);
    auto rev = get(edge_reverse, g);
    auto res = get(edge_residual_capacity, g);
    put(&color[0], sink, Color::gray());
    while (get(&color[0], sink) != Color::white()) {
      boost::queue<VD> Q;
      breadth_first_search
        (detail::residual_graph(g, res), source, Q,
         make_bfs_visitor(record_edge_predecessors(&pred[0], on_tree_edge())),
         &color[0]);
      if (get(&color[0], sink) != Color::white())
        detail::augment(g, source, sink, &pred[0], res, rev);
    } // while
}

}

#endif /* MIN_COST_MAX_FLOW_HPP */
