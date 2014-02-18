/**
 * @file multiway_cut_test.cpp
 * @brief
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2013-01-09
 */



#include <boost/test/unit_test.hpp>
#include <boost/range/irange.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "paal/greedy/k_cut/k_cut.hpp"
#include "paal/multiway_cut/multiway_cut.hpp"
#include "utils/logger.hpp"
#include "utils/test_result_check.hpp"

BOOST_AUTO_TEST_CASE(MultiwayCutS) {
    std::vector<std::pair<int,int> > edgesP{{0,3},{1,3},
                                            {0,4},{2,4},
                                            {1,5},{2,5},
                                            {3,6},{4,6},
                                            {3,7},{5,7},
                                            {4,8},{5,8},
                                            {6,7},{6,8},{7,8}

    };
    int nuVertices=6;
    std::vector<int> costEdges{100,100,100,100,100,100,10,10,10,10,10,10,1,1,1};

    std::vector<int> terminals={0,1,2};
    int optimal=320.0;
    boost::adjacency_list<boost::vecS,boost::vecS,boost::undirectedS,
                    boost::property < boost::vertex_color_t, int>,
                    boost::property < boost::edge_weight_t, int>
                    > graph(edgesP.begin(),edgesP.end(),costEdges.begin(),nuVertices);

    for(std::size_t i=1;i<=terminals.size();i++)
        put (boost::vertex_color,graph,terminals[i-1],i);

    auto weight= boost::get(boost::edge_weight, graph);


        std::vector<std::pair<int,int> > verticesParts;

    auto costCut=paal::multiway_cut(graph,back_inserter(verticesParts));
    LOGLN("cost cut: "<<costCut);
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


    check_result(costCut,optimal,2);
    LOGLN("Cost Cut Verification: "<<costCutVerification );
    BOOST_CHECK_EQUAL(costCut,costCutVerification);

}

