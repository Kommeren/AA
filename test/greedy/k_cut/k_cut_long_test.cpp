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
#include "utils/test_result_check.hpp"

const int nu_vertices=500;
const int nu_edges=1000*100;
const int seed=43;
const int parts=20;
const int max_edge_weight_in_components=10000;
const int max_edge_weight_between_components=10;
BOOST_AUTO_TEST_CASE(KCut) {
    LOGLN("wertices: "<<nu_vertices<<" edges: "<<nu_edges);
    LOGLN("parts: "<<parts);
    //generate graph
    std::vector<std::pair<int,int> > edgesP;
    std::vector<long long> costEdges;
    std::srand(seed);
    long long costCutOncomponents=0;
    {
        int source,target,edgeCost,nu_edgesCopy=nu_edges;
        while(--nu_edgesCopy){
            source=rand()%nu_vertices;
            target=rand()%nu_vertices;
            edgesP.push_back(std::make_pair(source,target));
            if(source%parts==target%parts){
                edgeCost=(rand()%max_edge_weight_in_components);
            }else{
                edgeCost=(rand()%max_edge_weight_between_components);
                costCutOncomponents+=edgeCost;
            }
            costEdges.push_back(edgeCost);
        }
    }
    boost::adjacency_list<boost::vecS,boost::vecS,boost::undirectedS,
                    boost::no_property,
                    boost::property < boost::edge_weight_t, int>
                    > graph(edgesP.begin(),edgesP.end(),costEdges.begin(),nu_vertices);
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
    BOOST_CHECK_EQUAL(costCut,costCutVerification);
	LOGLN("Number of parts: "<<parts);
    //estimate aproximation ratio
    check_result_compare_to_bound(costCutVerification,costCutOncomponents,2.0-2.0/double(parts),paal::utils::less_equal(),
                  0LL,"cut cost on components: ");
}
