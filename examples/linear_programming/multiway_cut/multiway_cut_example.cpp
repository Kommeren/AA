/**
 * @file multiway_cut_example.cpp
 * @brief 
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2013-11-13
 */

#include <boost/graph/adjacency_list.hpp>

#include "paal/greedy/k_cut/k_cut.hpp"
#include "paal/multiway_cut/multiway_cut.hpp"

int main(){
//! [Multiway Cut Example]
    
    // sample data
    
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
    boost::adjacency_list<boost::vecS,boost::vecS,boost::undirectedS,
                    boost::property < boost::vertex_color_t, int>,
                    boost::property < boost::edge_weight_t, int> 
                    > graph(edgesP.begin(),edgesP.end(),costEdges.begin(),nuVertices);

    for(std::size_t i=1;i<=terminals.size();i++)
        put (boost::vertex_color,graph,terminals[i-1],i);
    
    //solve
    
    std::vector<std::pair<int,int> > verticesParts;
    auto costCut=paal::multiway_cut(graph,back_inserter(verticesParts));
    
    //print result

    std::cout<<"cost cut: "<<costCut<<std::endl;
    std::cout<<"wertices (part)"<<std::endl;
    for(auto i:verticesParts){
        std::cout<<"  "<<i.first<<"      ( "<<i.second<<" )"<<std::endl;
    }
//! [Multiway Cut Example]
}
