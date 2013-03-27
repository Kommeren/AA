/**
 * @file min_cost_max_flow.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-03-18
 */
#ifndef MIN_COST_MAX_FLOW_HPP
#define MIN_COST_MAX_FLOW_HPP 

#define BOOST_RESULT_OF_USE_DECLTYPE

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

    typename std::decay<decltype(weight[*ei])>::type cost = 0;
    boost::tie(ei, end) = edges(g);
    for(;ei != end; ++ei) {
        if((capacity[*ei]) >  0) {
            cost +=  (capacity[*ei] - residual_capacity[*ei]) * weight[*ei];
        } 
    }
    return cost;
}


/*template <typename Graph>
void path_augmentation(Graph &g, typename graph_traits<Graph>::vertex_descriptor s, typename graph_traits<Graph>::vertex_descriptor t) {

}*/

template <typename ResidualGraph>
void path_augmentation_from_residual(ResidualGraph &g, typename graph_traits<ResidualGraph>::vertex_descriptor s, 
                                                       typename graph_traits<ResidualGraph>::vertex_descriptor t) {
    typedef typename property_map < ResidualGraph, edge_weight_t >::type Weight;
    typedef typename graph_traits < ResidualGraph>::edge_descriptor EdgeD;
    typedef typename property_traits<Weight>::value_type Dist;
    typedef typename property_map < ResidualGraph, edge_residual_capacity_t >::type ResidualCapacity;
    typedef typename property_map < ResidualGraph, edge_reverse_t >::type Reversed;
    typedef std::vector<unsigned> Pred;

    unsigned N = num_vertices(g);
    Weight weight = get(edge_weight, g);
    std::vector<Dist> distance(N);
    //TODO this vector shouldn't be needed
    std::vector<EdgeD> predE(N);
    Pred  pred(N);
    std::iota(pred.begin(), pred.end(), 0);
    ResidualCapacity residual_capacity = get(edge_residual_capacity, g);
    Reversed rev = get(edge_reverse, g);

    while(true) {
        std::iota(pred.begin(), pred.end(), 0);
        std::fill(distance.begin(), distance.end(),(std::numeric_limits<Dist>::max)());
        distance[s] = 0;
        bool b =  bellman_ford_shortest_paths(g, int(N), 
            weight_map(weight).distance_map(&distance[0]).predecessor_map(&pred[0]));

//        assert(b);
        if(pred[t] == t) {
            break;
        }

       for(unsigned i = 0; i < N; ++i) {
           predE[i] = edge(pred[i], i, g).first; 
       }

//DEBUG
     double delta;
     unsigned u;

    std::cout << "NOWA sciezka: :" <<std::endl;
      EdgeD e = predE[t];
      do {

        delta = std::min(double(delta), double(get(residual_capacity, e)));
        std::cout << e << " " << delta << " " << weight[e] <<std::endl;

        u = source(e, g);
        e = predE[u];
      } while (u != s);
//DEBUG
       detail::augment(g, s, t, &predE[0], residual_capacity, rev);
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

template <typename ResidualGraph>
void cycle_cancelation_from_residual(ResidualGraph &g) {
    typedef typename property_map < ResidualGraph, edge_weight_t >::type Weight;
    typedef std::vector<unsigned> Pred;
    typedef typename property_traits<Weight>::value_type Dist;
    typedef typename graph_traits < ResidualGraph>::edge_descriptor EdgeD;
    typedef typename property_map < ResidualGraph, edge_reverse_t >::type Reversed;
    typedef typename property_map < ResidualGraph, edge_residual_capacity_t >::type ResidualCapacity;
    typedef std::vector<unsigned> Pred;
    
    unsigned N = num_vertices(g);
    Weight weight = get(edge_weight, g);
    std::vector<Dist> distance(N);
    Pred  pred(N);
    ResidualCapacity residual_capacity = get(edge_residual_capacity, g);
    Reversed rev = get(edge_reverse, g);
    //TODO this vector shouldn't be needed
    std::vector<EdgeD> predE(N);
    
    std::iota(pred.begin(), pred.end(), 0);
    std::fill(distance.begin(), distance.end(), 0);
    while(!bellman_ford_shortest_paths(g, int(N), 
        weight_map(weight).distance_map(&distance[0]).predecessor_map(&pred[0]))) {

        unsigned cycleStart = find_cycle_start(g, &distance[0], &pred[0]);

        assert(pred[cycleStart] != cycleStart);
    
        for(unsigned i = 0; i < N; ++i) {
            predE[i] = edge(pred[i], i, g).first; 
        }
        detail::augment(g, cycleStart, cycleStart, &predE[0], residual_capacity, rev);
        
        std::iota(pred.begin(), pred.end(), 0);
        std::fill(distance.begin(), distance.end(),0);
    }
}

}

#endif /* MIN_COST_MAX_FLOW_HPP */
