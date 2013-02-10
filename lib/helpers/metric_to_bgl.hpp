#include <boost/graph/adjacency_matrix.hpp>

namespace paal {
//TODO it would be nice to adapt Matrix + something to bgl
    template <typename Metric, typename VertexIter> 
         boost::adjacency_matrix<boost::undirectedS, boost::no_property, 
                boost::property<boost::edge_weight_t, typename Metric::DistanceType> >

metricToBGL( const Metric & m, VertexIter vbegin, VertexIter vend) {
    typedef boost::adjacency_matrix<boost::undirectedS, boost::no_property, 
                boost::property<boost::edge_weight_t, typename Metric::DistanceType> > Graph;
    const unsigned N = std::distance(vbegin, vend);
    Graph g(N);
    return g;
}

} //paal
