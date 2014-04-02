/**
 * @file multiway_cut.hpp
 * @brief
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2013-12-19
 */

#ifndef MULTIWAY_CUT_HPP
#define MULTIWAY_CUT_HPP

#include "paal/lp/glp.hpp"

#include <boost/bimap.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/stoer_wagner_min_cut.hpp>
#include <boost/graph/named_function_params.hpp>

#include <fstream>
#include <tuple>
#include <utility>                   // for std::pair
#include <vector>


namespace paal{
namespace detail{
    inline int vertices_column_index(const int vertex,const int dimentions,const int column){
        return vertex*dimentions+column;
    }

    template<class Graph> using CostType =
            typename boost::property_traits<
                    puretype(get(boost::edge_weight,std::declval<Graph>()))
                    >::value_type;
    class multiway_cut_lp {
        public:
            ///Initialize the cut LP.
            template <typename Graph, typename LP>
            void init(Graph & graph, LP & lp,int k) {

                lp.set_lp_name("Multiway Cut");
                lp.set_optimization_type(lp::MINIMIZE);

                add_variables(graph, lp, k);
                add_constraints(graph, lp, k);
            }
        private:
            //adding variables
            //returns the number of variables
            template <typename Graph, typename LP>
            void add_variables(Graph & graph, LP & lp,int k) {
                auto weight=get(boost::edge_weight,graph);
                for(auto e : boost::make_iterator_range(edges(graph))) {
                    for(int i=0;i<k;++i){
                        auto colIdx = lp.add_column(weight(e));
                        edges_column.push_back(colIdx);
                    }
                }
                for(unsigned vertex=0;vertex<=num_vertices(graph);vertex++){
                    for(int i=0;i<k;++i){
                        auto colIdx = lp.add_column(0);
                        vertices_column.push_back(colIdx);
                    }
                }
            }

        template <typename Graph, typename LP>
            void add_constraints(Graph & graph, LP & lp,int k) {

                int dbIndex = 0;
                for(auto edge: boost::make_iterator_range(edges(graph))){
                    auto sour=source(edge,graph);
                    auto targ=target(edge,graph);
                    for(auto i: boost::irange(0,k)){
                        for(auto j:boost::irange(0,2)){
                            auto x_e = edges_column[vertices_column_index(dbIndex,k,i)];
                            auto x_src = vertices_column[vertices_column_index(sour,k,i)];
                            auto x_trg = vertices_column[vertices_column_index(targ,k,i)];
                            lp.add_row(x_e + (j*2-1) * x_src + (1-2*j) * x_trg >= 0);
                        }
                    }
                    ++dbIndex;
                }
                dbIndex = 0;
                for(auto vertex : boost::make_iterator_range(vertices(graph))){
                    auto col=get(boost::vertex_color,graph,vertex);
                    if(col!=0){
                        auto x_col = vertices_column[vertices_column_index(dbIndex,k,col-1)];
                        lp.add_row(x_col == 1);
                    }
                    lp::linear_expression expr;
                    for(auto i: boost::irange(0,k)){
                        expr += vertices_column[vertices_column_index(dbIndex,k,i)];
                    }
                    lp.add_row(std::move(expr) == 1);
                    ++dbIndex;
                }
            }
    public:
        std::vector<lp::col_id> edges_column;
        std::vector<lp::col_id> vertices_column;
    };



template <typename Graph,typename LP>
class multiway_cut{
public:
    multiway_cut_lp m_multiway_cut_lp;

    multiway_cut(const Graph & g,LP & lp,int k) :
                m_g(g),m_k(k){
                    m_multiway_cut_lp.init(g,lp,m_k);
                    lp.solve_simplex(lp::DUAL);
                };

     private:
            ///The input
            const Graph & m_g;
            int m_k;
};

template <typename Graph,typename Dist,typename Rand,typename Lp>
auto makeCut(const Graph & graph,int k,Dist &dist,Rand && random_engine,Lp &lp,multiway_cut_lp &mcLp,std::vector<int>& vertexToPart)->
        detail::CostType<Graph>{
    double cut_cost=0;
    auto weight=get(boost::edge_weight,graph);
    std::vector<double> randomRadiuses;
    dist.reset();
    for(int i=0;i<k;++i){
        randomRadiuses.push_back(dist(random_engine));
    }
    vertexToPart.resize(num_vertices(graph));
    auto index= get(boost::vertex_index, graph);
    auto get_column = [&] (int vertex,int dimension)->double {return lp.get_col_value(mcLp.vertices_column[vertices_column_index(vertex,k,dimension)]);};
    for(auto vertex:boost::make_iterator_range(vertices(graph))){
        for(int dimension=0;dimension<k;++dimension)
            if(1.0-get_column(index(vertex),dimension)<randomRadiuses[dimension] || dimension==k-1){
                //because each vertex have sum of coordinates equal 1, 1.0-get_column(vertex,dimension) is proportional to distance
                //to vertex correspond to dimension
                vertexToPart[index(vertex)]=dimension;
                break;
            }
    }
    for(auto edge: boost::make_iterator_range(edges(graph))){
        if(vertexToPart[index(source(edge,graph))]!=vertexToPart[index(target(edge,graph))])
            cut_cost+=weight(edge);
    }
    return cut_cost;
}





template <typename Rand=std::default_random_engine
         ,typename Distribution=std::uniform_real_distribution<double>
         ,typename LP=lp::glp
         ,typename Graph
         ,typename OutputIterator
         ,typename VertexIndexMap
         ,typename EdgeWeightMap
         ,typename VertexColorMap>
auto multiway_cut_dispatch(Graph &graph,
                OutputIterator result,
                Rand && random_engine,
                int iterations,
                VertexIndexMap indexMap,
                EdgeWeightMap weightMap,
                VertexColorMap colorMap)->
                typename boost::property_traits<EdgeWeightMap>::value_type
            {
                typedef detail::CostType<Graph> CostType;
                LP lp;
                Distribution dis(0,1);
                int terminals=0;
                for(auto vertex : boost::make_iterator_range(vertices(graph))){
                    terminals=std::max(terminals,colorMap(vertex));
                }
                detail::multiway_cut<Graph,LP> mc(graph,lp,terminals);
                CostType cut_cost=std::numeric_limits<CostType>::max();
                std::vector<int> best_solution;
                std::vector<int> solution;
                for(int i=0;i<iterations;++i){
                    solution.clear();
                    int res=detail::makeCut(graph,terminals,dis,random_engine,lp,mc.m_multiway_cut_lp,solution);
                    if(res<cut_cost){
                        swap(solution,best_solution);
                        cut_cost=res;
                    }
                }
                for(auto i:boost::make_iterator_range(vertices(graph))){
                    *result=std::make_pair(i,best_solution[indexMap(i)]);
                    ++result;
                }
                return cut_cost;

            }

template <typename Rand=std::default_random_engine
         ,typename Distribution=std::uniform_real_distribution<double>
         ,typename LP=lp::glp
         ,typename Graph
         ,typename OutputIterator
         ,typename VertexIndexMap
         ,typename EdgeWeightMap
         ,typename VertexColorMap>
auto multiway_cut_dispatch(Graph &graph,
                OutputIterator result,
                Rand && random_engine,
                boost::param_not_found,
                VertexIndexMap indexMap,
                EdgeWeightMap weightMap,
                VertexColorMap colorMap)->
                typename boost::property_traits<EdgeWeightMap>::value_type
            {
                int vertices=num_vertices(graph);
                const static int MIN_NUMBER_OF_REPEATS=100;
                auto numberOfRepeats = vertices*vertices + MIN_NUMBER_OF_REPEATS; //This variable is not supported by any proof
                return multiway_cut_dispatch(graph,result,random_engine,numberOfRepeats,indexMap,weightMap,colorMap);
            }

}//!detail

/**
 * @brief this is solve multiway_cut problem
 * and return cut_cost
 * example:
 *  \snippet multiway_cut_example.cpp  Multiway Cut Example
 * @param Graph graph
 * @param OutputIterator result pairs of vertex descriptor and number form (1,2, ... ,k) id of part
 * @param random_engine
 * @param params
 * @tparam Graph
 * @tparam OutputIterator
 * @tparam Rand random engine
 * @tparam Distribution used to chose random radius
 * @tparam LP
 * @tparam P
 * @tparam T
 * @tparam R
 */
template <typename Rand=std::default_random_engine
         ,typename Distribution=std::uniform_real_distribution<double>
         ,typename LP=lp::glp
         ,typename Graph
         ,typename OutputIterator
         ,typename P
         ,typename T
         ,typename R>
auto multiway_cut(const Graph &g, OutputIterator out,
        const boost::bgl_named_params<P, T, R>& params, Rand && random_engine=std::default_random_engine(5426u))->
        typename boost::property_traits<
                puretype(boost::choose_const_pmap(get_param(params, boost::edge_weight), g, boost::edge_weight))
                >::value_type {
    return detail::multiway_cut_dispatch(g, out,random_engine,
           get_param(params, boost::iterations_t()),
           boost::choose_const_pmap(get_param(params, boost::vertex_index), g, boost::vertex_index),
           boost::choose_const_pmap(get_param(params, boost::edge_weight), g, boost::edge_weight),
           boost::choose_const_pmap(get_param(params, boost::vertex_color), g, boost::vertex_color));
}

/**
 * @brief this is solve multiway_cut problem
 * and return cut_cost
 * example:
 *  \snippet multiway_cut_example.cpp  Multiway Cut Example
 *
 * complete example is  multiway_cut_example.cpp
 * @param Graph graph
 * @param int repeats number of sets of radius
 * @param OutputIterator result pairs of vertex descriptor and number form (1,2, ... ,k) id of part
 * @tparam Rand random engine
 * @tparam Distribution used to chose random radius
 * @tparam LP
 * @tparam Graph
 * @tparam OutputIterator
 */
template <typename Rand=std::default_random_engine
         ,typename Distribution=std::uniform_real_distribution<double>
         ,typename LP=lp::glp
         ,typename Graph
         ,class OutputIterator>
auto multiway_cut(Graph graph, OutputIterator result,Rand random_engine=std::default_random_engine(5426u))->
    detail::CostType<Graph> {
        return multiway_cut(graph,result,boost::no_named_parameters(),random_engine);
}

}//!paal
#endif /* MULTIWAY_CUT_HPP */
