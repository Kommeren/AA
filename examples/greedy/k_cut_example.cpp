/**
 * @file k_cut_example.cpp
 * @brief
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2013-08-30
 */


#include "paal/greedy/k_cut/k_cut.hpp"

#include <boost/graph/adjacency_list.hpp>

#include <iostream>

int main(){
//! [K Cut Example]
    // sample data
    std::vector<std::pair<int,int> > edgesP{{1,2},{1,5},{2,3},{2,5},{2,6},{3,4},{3,7},{4,7},{4,0},{5,6},{6,7},{7,0}};
    std::vector<int> costs{2,3,3,2,2,4,2,2,2,3,1,3};
    boost::adjacency_list<boost::vecS,boost::vecS,boost::undirectedS,
                    boost::no_property,
                    boost::property<boost::edge_index_t,std::size_t>
                    > graph(8);
    for(std::size_t i=0;i<edgesP.size();i++){
        add_edge(edgesP[i].first,edgesP[i].second,i,graph);
    }
    int parts=3;

    auto edgeId = get(boost::edge_index, graph);
    auto weight=make_iterator_property_map(costs.begin(), edgeId);

    //solve
    int costCut;
    std::vector<std::pair<int,int> > verticesParts;
    costCut=paal::greedy::kCut(graph,parts,back_inserter(verticesParts),boost::weight_map(weight));

    //alternative form
    //int costCut=paal::greedy::kCut(graph,parts,back_inserter(verticesParts));
    //this works if graph have edge weight property

    //print result
    std::cout<<"cost cut:"<<costCut<<std::endl;
    std::vector<int> verticesToParts;
    for(auto i:verticesParts){
        std::cout<<i.first<<"("<<i.second<<"), ";
    }
    std::cout<<std::endl;


//! [K Cut Example]
}
