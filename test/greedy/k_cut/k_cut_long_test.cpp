/**
 * @file k_cut_long_test.cpp
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

const int nuVertices=500;
const int nuEdges=1000*100;
const int seed=43;
const int parts=20;
const int maxEdgeWeightInComponents=10000;
const int maxEdgeWeightBetweenComponents=10;
BOOST_AUTO_TEST_CASE(KCut) {
    LOGLN("wertices: "<<nuVertices<<" edges: "<<nuEdges);
    LOGLN("parts: "<<parts);
    //generate graph
    std::vector<std::pair<int,int> > edgesP;
    std::vector<long long> costEdges;
    std::srand(seed);
    long long costCutOnComponents=0;
    {
        int source,target,edgeCost,nuEdgesCopy=nuEdges;
        while(--nuEdgesCopy){
            source=rand()%nuVertices;
            target=rand()%nuVertices;
            edgesP.push_back(std::make_pair(source,target));
            if(source%parts==target%parts){
                edgeCost=(rand()%maxEdgeWeightInComponents);
            }else{
                edgeCost=(rand()%maxEdgeWeightBetweenComponents);
                costCutOnComponents+=edgeCost;
            }
            costEdges.push_back(edgeCost);
        }
    }
    boost::adjacency_list<boost::vecS,boost::vecS,boost::undirectedS,
                    boost::no_property,
                    boost::property < boost::edge_weight_t, int> 
                    > graph(edgesP.begin(),edgesP.end(),costEdges.begin(),nuVertices);
    //solve
    std::vector<std::pair<int,int> > verticesParts;
    long long costCut=paal::greedy::kCut(graph,parts,back_inserter(verticesParts));
    //print result
    LOGLN("cost cut: "<<costCut);
    std::vector<int> verticesToParts;
    verticesToParts.resize(verticesParts.size());
    for(auto i:verticesParts){
        LOG(i.first<<"("<<i.second<<"), ");
        verticesToParts[i.first]=i.second;
    }
    LOGLN("");
    //verificate result
    auto weight= get(boost::edge_weight, graph); 
    long long costCutVerification=0;
    auto allEdges=edges(graph);
    for(auto edge : boost::make_iterator_range(allEdges)){
        if(verticesToParts[source(edge,graph)]!=verticesToParts[target(edge,graph)])
            costCutVerification+=weight(edge);
    }
    //estimate aproximation ratio
    LOGLN("cost cut from algorithm: "<<costCutVerification <<" cut cost on "<<parts<<" components: "<<costCutOnComponents);
    auto lowerBoundAproxmiationRatio=(costCutVerification)/double(costCutOnComponents);
    LOGLN("Aproxmimation Ratio is not beter than: "<<lowerBoundAproxmiationRatio);
    BOOST_CHECK(lowerBoundAproxmiationRatio<=2.0-2.0/double(parts));
    LOGLN("");
    BOOST_CHECK_EQUAL(costCut,costCutVerification);

}
