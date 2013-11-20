/**
 * @file k_cut_test.cpp
 * @brief 
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2013-08-20
 */

#include <boost/test/unit_test.hpp>
#include <boost/range/irange.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "paal/greedy/k_cut/k_cut.hpp"
#include "utils/logger.hpp"

BOOST_AUTO_TEST_CASE(KCut) {
    std::vector<std::pair<int,int> > edgesP{{1,2},{1,5},{2,3},{2,5},{2,6},{3,4},{3,7},{4,7},{4,0},{5,6},{6,7},{7,0}};
    std::vector<int> cost{2,3,3,2,2,4,2,2,2,3,1,3};
    boost::adjacency_list<boost::vecS,boost::vecS,boost::undirectedS,
                    boost::no_property,
                    boost::property < boost::edge_weight_t, int> 
                    > graph(edgesP.begin(),edgesP.end(),cost.begin(),8);
    std::vector<int> optimal={4,9,13,16,21,25,29};

    auto weight= boost::get(boost::edge_weight, graph); 
    
    for(auto i:boost::irange(2,9)){
        std::vector<std::pair<int,int> > verticesParts;
        int costCut=paal::greedy::kCut(graph,i,back_inserter(verticesParts));
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
        LOGLN("cost cut: "<<costCutVerification <<" optimal: "<<optimal[i-2]);
        LOGLN("Aproximation Ratio: "<<double(costCutVerification)/optimal[i-2]);
        LOGLN("");
        BOOST_CHECK_EQUAL(costCut,costCutVerification);
        BOOST_CHECK(costCut>=optimal[i-2]);
    }

}
