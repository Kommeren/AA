/**
 * @file k_cut.hpp
 * @brief 
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2013-09-25
 */
#ifndef K_CUT
#define K_CUT

#include <queue>


#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/stoer_wagner_min_cut.hpp>
#include <boost/graph/one_bit_color_map.hpp>
#include <boost/graph/subgraph.hpp>
#include <boost/graph/iteration_macros.hpp>
#include <boost/graph/copy.hpp>
#include "paal/utils/type_functions.hpp"

namespace paal{
namespace greedy{
namespace k_cut{
/**
 * @brief this is solve k_cut problem
 * and return cut_cost
 * example: 
 *  \snippet k_cut_example.cpp K Cut Example
 *
 * complete example is k_cut.cpp
 * @param Graph graph
 * @param int numberOfParts
 * @param OutputIterator result
 * @tparam inGraph
 * @tparam OutputIterator
 */
template<typename inGraph, class OutputIterator>
auto kCut(const inGraph& g,int numberOfParts,OutputIterator result) ->
typename boost::property_traits<puretype(get(boost::edge_weight,g))>::value_type{
    typedef puretype(get(boost::edge_weight,g)) Weight;
    typedef typename boost::property_traits<Weight>::value_type cost_t;
    assert(num_vertices(g)>=numberOfParts);
    typedef boost::subgraph<
            boost::adjacency_list<
                    boost::vecS,boost::vecS,boost::undirectedS,
                    boost::no_property,
                    boost::property < boost::edge_weight_t, int , boost::property<boost::edge_index_t, int> > 
                    > > Graph;
    std::vector<Graph *> parts;
    //cuts contain pair(x,y) 
    // x is a cost cut 
    // y and y+1 are index parts of graph after make a cut
    std::priority_queue<std::pair<int,int> >cuts;
    Graph G0(boost::num_vertices(g));
    {
        auto weight= get(boost::edge_weight, g);
        for(auto edge : make_iterator_range(edges(g))){
            add_edge(source(edge,g),target(edge,g),weight(edge),G0);
        }
    }
    int idPart=0;
    
    //get part id and compute minimum cost of cut of that part and add it to queue
    auto makeCut=[&](int id){
        Graph& G0 = *parts[id];
        if(num_vertices(G0)<2){
            ++idPart;
            *result=std::make_pair((G0.local_to_global(*vertices(G0).first)),idPart);
            ++result;
            return;
        }
        Graph& G1 = parts[id]->create_subgraph();
        Graph& G2 = parts[id]->create_subgraph();
        parts.push_back(&G1);
        parts.push_back(&G2);
        auto parities=boost::make_one_bit_color_map(num_vertices(G0), get(boost::vertex_index, G0));
        auto cutCost=boost::stoer_wagner_min_cut(G0,get(boost::edge_weight, G0), boost::parity_map(parities));
        for (auto i :  boost::irange(0,int(boost::num_vertices(G0)))) {
            int w=G0.local_to_global(i);
            if (get(parities, i)){
                add_vertex(w,G1);
            }
            else{
                add_vertex(w,G2);
            }
        }
        cuts.push(std::make_pair(-cutCost,parts.size()-2));
    };
    
    parts.push_back(&G0);
    makeCut(0);
    cost_t kCutCost=cost_t();
    while(--numberOfParts){
        auto cut=cuts.top();
        cuts.pop();
        kCutCost-=cut.first;
        makeCut(cut.second);
        makeCut(cut.second+1);
    }
    auto assignVerticesFromCutToPart=[&](int id){
        BGL_FORALL_VERTICES_T(vi,(*parts[id]),Graph){
            *result=std::make_pair(((*parts[id]).local_to_global(vi)),idPart);
            ++result;
        }
    };
    while(!cuts.empty()){
        auto cut=cuts.top();
        cuts.pop();
        ++idPart;
        assignVerticesFromCutToPart(cut.second);
        assignVerticesFromCutToPart(cut.second+1);
    }
    return kCutCost;
}

}//!k_cut
}//!greedy
}//!paal

#endif /* K_CUT */