/**
 * @file read_orlib_km.hpp
 * @brief 
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2013-08-01
 */
#ifndef READ_ORLIB_KM_HPP
#define READ_ORLIB_KM_HPP

#include <type_traits>

#include "paal/data_structures/metric/graph_metrics.hpp"
#include "utils/logger.hpp"
#include <map>
namespace paal {

namespace cap {
    class uncapacitated;
    class capacitated;
}


template <typename IsCapacitated, typename IStream>
typename std::enable_if<std::is_same<IsCapacitated, cap::uncapacitated>::value, int>::type  
readDemand(IStream & i){
    int a;
    i >> a;
    return 1;
}

template <typename IsCapacitated, typename IStream>
typename std::enable_if<std::is_same<IsCapacitated, cap::capacitated>::value, int>::type
readDemand(IStream & ist){
    int a;
    ist >> a;
    return a;
}

typedef boost::adjacency_list <boost::vecS, boost::vecS, boost::undirectedS, boost::no_property, boost::property < boost::edge_weight_t, int > > graph_t;
typedef std::pair<int,int> Edge;
typedef paal::data_structures::GraphMetric<graph_t, int> GraphMT;

template <typename IsCapacitated>
GraphMT readORLIB_KM(std::istream & ist, std::vector<long long> & facCosts, std::vector<int> & facCap,
                       std::vector<int> & demands,
                       boost::integer_range<int> & fac,
                       boost::integer_range<int> & clients ) {
    int V, nE, K;
    ist >> V >> nE >> K;
    
    fac =     boost::irange(0, K);
    clients = boost::irange(K, V);
    
    data_structures::ArrayMetric<long long> m(V);
    demands.resize(V);
    facCosts.resize(V);
    facCap.resize(V);
    
    int a,b;
    int l;
    std::vector<Edge> edges;
    std::vector<int> weight;
    for(int i : boost::irange(0,V)) {
        facCosts[i] = 0;
        demands[i] = 1;
    }
    std::map<std::pair<int,int> ,int> mapa;
    int j=1;
    for(int i : boost::irange(0,nE)) {
        ist >> a>>b>>l;
        if(a<b)
          std::swap(a,b);
        if(mapa[std::make_pair(a,b)]>i+1)
          LOG("error");
        if(mapa[std::make_pair(a,b)]==0){
          mapa[std::make_pair(a,b)]=j;
          j++;
        edges.push_back(Edge(a-1,b-1));
        weight.push_back(l);
        }else{ 
          weight[mapa[std::make_pair(a,b)]-1]=l;
        }
    }
    graph_t g(edges.begin(), edges.end(), weight.begin(), V);
    return data_structures::GraphMetric<graph_t, int>(g);   
}

}
#endif /* READ_ORLIB_FC_HPP */
