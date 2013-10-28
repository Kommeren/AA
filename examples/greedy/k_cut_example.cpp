/**
 * @file k_cut_example.cpp
 * @brief 
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2013-08-30
 */

#include <iostream>
#include <boost/graph/adjacency_list.hpp>

#include "paal/greedy/k_cut/k_cut.hpp"

int main(){
//! [K Cut Example]
    // sample data
    std::vector<std::pair<int,int> > edgesP{{1,2},{1,5},{2,3},{2,5},{2,6},{3,4},{3,7},{4,7},{4,0},{5,6},{6,7},{7,0}};
    std::vector<int> cost{2,3,3,2,2,4,2,2,2,3,1,3};
    boost::adjacency_list<boost::vecS,boost::vecS,boost::undirectedS,
                    boost::no_property,
                    boost::property < boost::edge_weight_t, int> 
                    > graph(edgesP.begin(),edgesP.end(),cost.begin(),8);
    int parts=3;
    //solve
    std::vector<std::pair<int,int> > verticesParts;
    int costCut=paal::greedy::kCut(graph,parts,back_inserter(verticesParts));
    
    //print result
    std::cout<<"cost cut:"<<costCut<<std::endl;
    std::vector<int> verticesToParts;
    for(auto i:verticesParts){
        std::cout<<i.first<<"("<<i.second<<"), ";
    }
    std::cout<<std::endl;
    
//! [K Cut Example]}
}