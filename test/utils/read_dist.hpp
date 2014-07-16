/**
* @file read_dist.hpp
* @brief
* @author Jakub Oćwieja
* @version 1.0
* @date 2014-05-19
*/

#ifndef READ_DIST_HPP
#define READ_DIST_HPP

#include<boost/graph/adjacency_list.hpp>

namespace paal {
namespace {

typedef  boost::adjacency_list<
        boost::vecS,
        boost::vecS,
        boost::undirectedS,
        boost::no_property,
        boost::property < boost::edge_weight_t, int> >
    Graph;

Graph read_dist(std::istream& ist) {
    int n, m; ist >> n >> m;
    std::vector< std::pair<int, int> > edges(m);
    std::vector< int > edgeWeights(m);

    for (auto i: boost::irange(0,m)) {
        ist >> edges[i].first >> edges[i].second >> edgeWeights[i];
    }

    return Graph(edges.begin(), edges.end(), edgeWeights.begin(), n);
}

}
} // paal

#endif /* READ_DIST_HPP */
