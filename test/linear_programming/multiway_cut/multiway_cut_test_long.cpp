/**
 * @file multiway_cut_test_long.cpp
 * @brief
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2014-01-17
 */

#include <boost/test/unit_test.hpp>

#include "utils/logger.hpp"
#include "utils/test_result_check.hpp"

#include "paal/multiway_cut/multiway_cut.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/range/irange.hpp>

#include <vector>

namespace{
template <typename Graph>
double test(Graph graph){
    auto weight= boost::get(boost::edge_weight, graph);
    std::vector<std::pair<int,int> > vertices_parts;
    LOGLN("multiway_cut: ");
    auto cost_cut=paal::multiway_cut(graph,back_inserter(vertices_parts));
    std::vector<int> vertices_to_parts;
    vertices_to_parts.resize(vertices_parts.size());
    for(auto i:vertices_parts){
        LOG(i.first<<"("<<i.second<<"), ");
        vertices_to_parts[i.first]=i.second;
    }
    LOGLN("");
    int cost_cut_verification=0;
    auto all_edges=edges(graph);
    for(auto i=all_edges.first;i!=all_edges.second;i++){
        if(vertices_to_parts[source(*i,graph)]!=vertices_to_parts[target(*i,graph)])
            cost_cut_verification+=weight(*i);
    }

    LOGLN("Cost Cut:              "<<cost_cut);
    LOGLN("Cost Cut Verification: "<<cost_cut_verification );
    LOGLN("");
    BOOST_CHECK_EQUAL(cost_cut,cost_cut_verification);
    return cost_cut;
}
auto env_to_add_edge =[](std::vector<std::pair<int,int> >& edges,std::vector<long long>& cost_edges){
    return [&](int source,int target,int edge_cost){
        edges.push_back(std::make_pair(source,target));
        cost_edges.push_back(edge_cost);
        LOGLN(source<<" "<<target<<" "<<edge_cost);
    };
};

}
BOOST_AUTO_TEST_SUITE(multiway_cut)


BOOST_AUTO_TEST_CASE(multiway_cut_random) {
    static const int NU_VERTICES=1000;
    static const int NU_RANDOM_EDGES=1000;
    static const int NU_TERMINAL_EDGES=1000;
    static const int NU_COMPONENTS_EDGES=1000;
    static const int SEED=211;
    static const int CONECT_TO_TERMINAL_COST=1000*1000;
    static const int CONECT_IN_COMPONENT_COST=1000;
    static const int CONECT_BETWEEN_COMPONENTS_COST=1;
    static const int NU_COMPONENTS=3;
    std::vector<std::pair<int,int> > edges_p;
    std::vector<long long> cost_edges;
    std::srand(SEED);
    auto add_edge_to_graph=env_to_add_edge(edges_p,cost_edges);
    auto rand_vertex_id=[&](){
        return rand()%NU_VERTICES;
    };
    auto rand_vertex_id_in_component=[&](int component){
        return (((rand_vertex_id())/NU_COMPONENTS)*NU_COMPONENTS+component)%NU_VERTICES;
    };
    {
        //add edges to terminals
        int source,target,nu_edges_copy=NU_TERMINAL_EDGES;
        while(--nu_edges_copy){
            do{
                source=rand_vertex_id();
                target=source%NU_COMPONENTS;
            }while(source==target);
            add_edge_to_graph(source,target,CONECT_TO_TERMINAL_COST);
        }

        //add edges in components
        nu_edges_copy=NU_COMPONENTS_EDGES;
        while(--nu_edges_copy){
            do{
                source=rand_vertex_id();
                target=rand_vertex_id_in_component(source);
            }while(source==target);
            add_edge_to_graph(source,target,CONECT_IN_COMPONENT_COST);
        }

        //add random edges
        nu_edges_copy=NU_RANDOM_EDGES;
        while(--nu_edges_copy){
            do{
                source=rand_vertex_id();
                target=rand_vertex_id();
            }while(source==target);
            add_edge_to_graph(source,target,CONECT_BETWEEN_COMPONENTS_COST);
        }
    }
    std::vector<int> terminals={0,1,2};
    boost::adjacency_list<boost::vecS,boost::vecS,boost::undirectedS,
                    boost::property < boost::vertex_color_t, int>,
                    boost::property < boost::edge_weight_t, int>
                    > graph(edges_p.begin(),edges_p.end(),cost_edges.begin(),NU_VERTICES);

    for(std::size_t i=1;i<=terminals.size();i++)
        put (boost::vertex_color,graph,terminals[i-1],i);
    ::test(graph);
}

BOOST_AUTO_TEST_CASE(multiway_cut_triangle) {
    //We place 33*33 points on the grid
    //each point above diagonal have edge to right, down and left-down neighbor
    //each point on diagonal have edge to left-down neighbor
    //
    //  *-*-*-*-*
    //  |/|/|/|/
    //  *-*-*-* *
    //  |/|/|/
    //  *-*-* * *
    //  |/|/
    //  *-* * * *
    //  |/
    //  * * * * *
    //
    // in the oder words we have triangle
    // and we several times get all triangle add point on each edge and connect them dividing triangle on 4 triangle
    // edges added in each step have ten times smaller cost

    static const int SIZ=33;
    static const int OPTIMAL=324992;
    static const int NU_VERTICES=SIZ*SIZ;
    static const int SEED=211;
    std::vector<std::pair<int,int> > edges_p;
    std::vector<long long> cost_edges;
    std::srand(SEED);
    auto coordinates= [&](int x,int y){return x*SIZ+y;};
    auto size_range=boost::irange(0,SIZ);
    auto add_edge_to_graph=env_to_add_edge(edges_p,cost_edges);
    auto gen_cost=[](int position){
        long long cost=1;
        while(position%2==0){
            position/=2;
            cost*=10;
        }
        return cost;
    };
    {
        for(auto i:size_range)
        for(auto j:size_range){
            if(i+j<SIZ-1){
                //add horizontal edges
                add_edge_to_graph(coordinates(i,j),coordinates(i,j+1),gen_cost(i+SIZ-1));
                //add vertical edges
                add_edge_to_graph(coordinates(i,j),coordinates(i+1,j),gen_cost(j+SIZ-1));
            }
            if(i+j<SIZ&&i>0){
                //add diagonal edges
                add_edge_to_graph(coordinates(i,j),coordinates(i-1,j+1),gen_cost(i+j));
            }
        }
    }
    std::vector<int> terminals={0,SIZ-1,(SIZ-1)*SIZ};
    boost::adjacency_list<boost::vecS,boost::vecS,boost::undirectedS,
                    boost::property < boost::vertex_color_t, int>,
                    boost::property < boost::edge_weight_t, int>
                    > graph(edges_p.begin(),edges_p.end(),cost_edges.begin(),NU_VERTICES);

    for(std::size_t i=1;i<=terminals.size();i++)
        put (boost::vertex_color,graph,terminals[i-1],i);

    int cost_cut=::test(graph);
    check_result(cost_cut,OPTIMAL,2);
}
BOOST_AUTO_TEST_SUITE_END();
