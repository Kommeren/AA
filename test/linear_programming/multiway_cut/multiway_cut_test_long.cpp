/**
 * @file multiway_cut_test_long.cpp
 * @brief
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2014-01-17
 */


#include <vector>

#include <boost/test/unit_test.hpp>
#include <boost/range/irange.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "paal/greedy/k_cut/k_cut.hpp"
#include "paal/multiway_cut/multiway_cut.hpp"
#include "utils/logger.hpp"

namespace{
template <typename Graph>
double test(Graph graph){
    auto weight= boost::get(boost::edge_weight, graph);
    std::vector<std::pair<int,int> > verticesParts;
    LOGLN("multiway_cut: ");
    auto costCut=paal::multiway_cut(graph,back_inserter(verticesParts));
    std::vector<int> verticesToParts;
    verticesToParts.resize(verticesParts.size());
    for(auto i:verticesParts){
        LOG(i.first<<"("<<i.second<<"), ");
        verticesToParts[i.first]=i.second;
    }
    LOGLN("");
    int costCutVerification=0;
    auto allEdges=edges(graph);
    for(auto i=allEdges.first;i!=allEdges.second;i++){
        if(verticesToParts[source(*i,graph)]!=verticesToParts[target(*i,graph)])
            costCutVerification+=weight(*i);
    }

    LOGLN("Cost Cut:              "<<costCut);
    LOGLN("Cost Cut Verification: "<<costCutVerification );
    LOGLN("");
    BOOST_CHECK_EQUAL(costCut,costCutVerification);
    return costCut;
}
auto envToAddEdge =[](std::vector<std::pair<int,int> >& edges,std::vector<long long>& costEdges){
    return [&](int source,int target,int edgeCost){
        edges.push_back(std::make_pair(source,target));
        costEdges.push_back(edgeCost);
        LOGLN(source<<" "<<target<<" "<<edgeCost);
    };
};

}
BOOST_AUTO_TEST_SUITE(MultiwayCut)


BOOST_AUTO_TEST_CASE(MultiwayCutRandom) {
    static const int NU_VERTICES=1000;
    static const int NU_RANDOM_EDGES=1000;
    static const int NU_TERMINAL_EDGES=1000;
    static const int NU_COMPONENTS_EDGES=1000;
    static const int SEED=211;
    static const int CONECT_TO_TERMINAL_COST=1000*1000;
    static const int CONECT_IN_COMPONENT_COST=1000;
    static const int CONECT_BETWEEN_COMPONENTS_COST=1;
    static const int NU_COMPONENTS=3;
    std::vector<std::pair<int,int> > edgesP;
    std::vector<long long> costEdges;
    std::srand(SEED);
    auto addEdge=envToAddEdge(edgesP,costEdges);
    auto randVertexID=[&](){
        return rand()%NU_VERTICES;
    };
    auto randVertexIDinComponent=[&](int component){
        return (((randVertexID())/NU_COMPONENTS)*NU_COMPONENTS+component)%NU_VERTICES;
    };
    {
        //add edges to terminals
        int source,target,nuEdgesCopy=NU_TERMINAL_EDGES;
        while(--nuEdgesCopy){
            do{
                source=randVertexID();
                target=source%NU_COMPONENTS;
            }while(source==target);
            addEdge(source,target,CONECT_TO_TERMINAL_COST);
        }

        //add edges in components
        nuEdgesCopy=NU_COMPONENTS_EDGES;
        while(--nuEdgesCopy){
            do{
                source=randVertexID();
                target=randVertexIDinComponent(source);
            }while(source==target);
            addEdge(source,target,CONECT_IN_COMPONENT_COST);
        }

        //add random edges
        nuEdgesCopy=NU_RANDOM_EDGES;
        while(--nuEdgesCopy){
            do{
                source=randVertexID();
                target=randVertexID();
            }while(source==target);
            addEdge(source,target,CONECT_BETWEEN_COMPONENTS_COST);
        }
    }
    std::vector<int> terminals={0,1,2};
    boost::adjacency_list<boost::vecS,boost::vecS,boost::undirectedS,
                    boost::property < boost::vertex_color_t, int>,
                    boost::property < boost::edge_weight_t, int>
                    > graph(edgesP.begin(),edgesP.end(),costEdges.begin(),NU_VERTICES);

    for(std::size_t i=1;i<=terminals.size();i++)
        put (boost::vertex_color,graph,terminals[i-1],i);
    ::test(graph);
}

BOOST_AUTO_TEST_CASE(MultiwayCutTriangle) {
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
    std::vector<std::pair<int,int> > edgesP;
    std::vector<long long> costEdges;
    std::srand(SEED);
    auto coordinates= [&](int x,int y){return x*SIZ+y;};
    auto sizeRange=boost::irange(0,SIZ);
    auto addEdge=envToAddEdge(edgesP,costEdges);
    auto genCost=[](int position){
        long long cost=1;
        while(position%2==0){
            position/=2;
            cost*=10;
        }
        return cost;
    };
    {
        for(auto i:sizeRange)
        for(auto j:sizeRange){
            if(i+j<SIZ-1){
                //add horizontal edges
                addEdge(coordinates(i,j),coordinates(i,j+1),genCost(i+SIZ-1));
                //add vertical edges
                addEdge(coordinates(i,j),coordinates(i+1,j),genCost(j+SIZ-1));
            }
            if(i+j<SIZ&&i>0){
                //add diagonal edges
                addEdge(coordinates(i,j),coordinates(i-1,j+1),genCost(i+j));
            }
        }
    }
    std::vector<int> terminals={0,SIZ-1,(SIZ-1)*SIZ};
    boost::adjacency_list<boost::vecS,boost::vecS,boost::undirectedS,
                    boost::property < boost::vertex_color_t, int>,
                    boost::property < boost::edge_weight_t, int>
                    > graph(edgesP.begin(),edgesP.end(),costEdges.begin(),NU_VERTICES);

    for(std::size_t i=1;i<=terminals.size();i++)
        put (boost::vertex_color,graph,terminals[i-1],i);

    double costCut=::test(graph);

    LOGLN("Cost Cut Optimal: "<<OPTIMAL );
    BOOST_CHECK(costCut>=OPTIMAL);
    LOGLN("Aproximation Ratio: "<<double(costCut)/OPTIMAL);
}
BOOST_AUTO_TEST_SUITE_END();
