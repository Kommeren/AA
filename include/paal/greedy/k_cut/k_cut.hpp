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
#include "paal/utils/functors.hpp"

namespace paal{
namespace greedy{

/**
 * @brief this is solve k_cut problem
 * and return cut_cost
 * example:
 *  \snippet k_cut_example.cpp K Cut Example
 *
 * complete example is k_cut_example.cpp
 * @param Graph graph
 * @param int numberOfParts
 * @param OutputIterator result pairs of vertex_descriptor and number form (1,2, ... ,k) id of part
 * @param VertexIndexMap indexMap
 * @param EdgeWeightMap weightMap
 * @tparam inGraph
 * @tparam OutputIterator
 * @tparam VertexIndexMap
 * @tparam EdgeWeightMap
 */
template<typename inGraph, class OutputIterator, typename VertexIndexMap, typename EdgeWeightMap>
auto kCut(const inGraph& graph,unsigned int numberOfParts,OutputIterator result,
            VertexIndexMap indexMap, EdgeWeightMap weightMap) ->
            typename boost::property_traits<EdgeWeightMap>::value_type{
    typedef typename boost::property_traits<EdgeWeightMap>::value_type cost_t;

    typedef boost::adjacency_list<
            boost::vecS,boost::vecS,boost::undirectedS,
            boost::no_property,
            boost::property < boost::edge_weight_t, cost_t ,
                boost::property<boost::edge_index_t, int> >
            >Graph;

    assert(num_vertices(graph)>=numberOfParts);

    std::vector<int> vertexToPart(num_vertices(graph));
    typedef typename std::vector<int> VertexIndexToVertexIndex;
    VertexIndexToVertexIndex vertexInSubgraphToVertex(num_vertices(graph));
    VertexIndexToVertexIndex vertexToVertexInSubgraph(num_vertices(graph));
    int vertexInPart;
    int parts=1;
    //cuts contain pair(x,y)
    // x is the cost of the cut
    // y and y+1 are index parts of graph after make a cut
    std::priority_queue<
            std::pair<cost_t,int>,
            std::vector<std::pair<cost_t,int> >
            ,utils::Greater> cuts;

    int idPart=0;

    //get part id and compute minimum cost of cut of that part and add it to queue
    auto makeCut=[&](int id){

        vertexInPart=0;
        for(auto i:boost::make_iterator_range(vertices(graph))){
            if(vertexToPart[indexMap(i)]==id){
                vertexInSubgraphToVertex[vertexInPart]=indexMap(i);
                vertexToVertexInSubgraph[indexMap(i)]=vertexInPart;
                ++vertexInPart;

            }
        }
        Graph part(vertexInPart);
        for(auto edge : boost::make_iterator_range(edges(graph))){
            auto sour=indexMap(source(edge,graph));
            auto targ=indexMap(target(edge,graph));
            if(vertexToPart[sour]==id &&
                    vertexToPart[targ]==id &&
                    sour!=targ){
                add_edge(vertexToVertexInSubgraph[sour],
                         vertexToVertexInSubgraph[targ],
                         get(weightMap,edge),
                         part);
            }
        }
        if(vertexInPart<2){
            ++idPart;
            *result=std::make_pair(vertexInSubgraphToVertex[0],idPart);
            ++result;
            return;
        }
        auto parities=boost::make_one_bit_color_map(num_vertices(part),
                                                    get(boost::vertex_index,part));
        auto cutCost=boost::stoer_wagner_min_cut(part,
                                          get(boost::edge_weight, part),
                                          boost::parity_map(parities));

        for (auto i :  boost::irange(0,int(num_vertices(part)))) {
            vertexToPart[vertexInSubgraphToVertex[i]]=
                    parts+get(parities, i);//return value convertable to 0/1
        }
        cuts.push(std::make_pair(cutCost,parts));
        parts+=2;
    };

    makeCut(0);
    cost_t kCutCost=cost_t();
    while(--numberOfParts){
        auto cut=cuts.top();
        cuts.pop();
        kCutCost+=cut.first;
        makeCut(cut.second);
        makeCut(cut.second+1);
    }

    while(!cuts.empty()){
        auto cut=cuts.top();
        cuts.pop();
        ++idPart;
        for(auto i:boost::make_iterator_range(vertices(graph))){
            if(vertexToPart[indexMap(i)]==cut.second ||
                    vertexToPart[indexMap(i)]==cut.second+1){
                *result=std::make_pair(i,idPart);
                ++result;
            }
        }
    }
    return kCutCost;
}

/**
 * @brief this is solve k_cut problem
 * and return cut_cost
 * example:
 *  \snippet k_cut_example.cpp K Cut Example
 *
 * complete example is k_cut_example.cpp
 * @param Graph graph
 * @param int numberOfParts
 * @param OutputIterator result pairs of vertex_descriptor and number form (1,2, ... ,k) id of part
 * @param params
 * @param EdgeWeightMap weightMap
 * @tparam inGraph
 * @tparam OutputIterator
 * @tparam T
 * @tparam P
 * @tparam R
 */
template<typename inGraph
        ,class OutputIterator
        ,typename T
        ,typename P
        ,typename R>
auto kCut(const inGraph& graph,unsigned int numberOfParts,OutputIterator result,const boost::bgl_named_params<P, T, R>& params) ->
        typename boost::property_traits<
            puretype(boost::choose_const_pmap(get_param(params, boost::edge_weight), graph, boost::edge_weight))
            >::value_type{
    return kCut(graph,numberOfParts,result,
        boost::choose_const_pmap(get_param(params, boost::vertex_index),graph,boost::vertex_index),
        boost::choose_const_pmap(get_param(params, boost::edge_weight),graph,boost::edge_weight)
    );
}

/**
 * @brief this is solve k_cut problem
 * and return cut_cost
 * example:
 *  \snippet k_cut_example.cpp K Cut Example
 *
 * complete example is k_cut_example.cpp
 * @param Graph graph
 * @param int numberOfParts
 * @param OutputIterator result pairs of vertex_descriptor and number form (1,2, ... ,k) id of part
 * @tparam inGraph
 * @tparam OutputIterator
 */
template<typename inGraph,class OutputIterator>
auto kCut(const inGraph& graph,unsigned int numberOfParts,OutputIterator result) ->
        typename boost::property_traits<puretype(get(boost::edge_weight,graph))>::value_type{
    return kCut(graph,numberOfParts,result,boost::no_named_parameters());
}

}//!greedy
}//!paal

#endif /* K_CUT */