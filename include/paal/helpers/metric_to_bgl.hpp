#include <boost/graph/adjacency_matrix.hpp>

#include "paal/data_structures/bimap.hpp"
#include "paal/helpers/metric_on_idx.hpp"

namespace paal {
namespace helpers {
//TODO it would be nice to adapt Matrix + something to bgl
    
template <typename Metric>  struct AdjacencyMatrix {
    typedef boost::adjacency_matrix<boost::undirectedS, boost::no_property, 
                boost::property<boost::edge_weight_t, typename Metric::DistanceType> > type;
};


 template <typename Metric, typename VertexIter>                
 typename   AdjacencyMatrix<Metric>::type
 /**
  * @brief we assume that (vbegin, vend) is sequence of values  (0, vend - vbegin).
  *
  * @param m
  * @param vbegin
  * @param vend
  */
metricToBGL( const Metric & m, VertexIter vbegin, VertexIter vend) {
    typedef typename AdjacencyMatrix<Metric>::type Graph;
    const unsigned N = std::distance(vbegin, vend);
    typedef typename Metric::VertexType VertexType;
    typedef typename Metric::DistanceType Dist;
    Graph g(N);
    std::for_each(vbegin, vend, [&](VertexType v){
        std::for_each(vbegin, vend, [&](VertexType w){
            if(v < w) {
                bool succ = boost::add_edge(v, w, 
                        boost::property<boost::edge_weight_t, Dist>(m(v,w)), g).second;
                assert(succ);
            }
        });     
    });
    return g;
}
 
template <typename Metric, typename VertexIter>                
 typename   AdjacencyMatrix<Metric>::type
metricToBGLWithIndex(const Metric & m, VertexIter vbegin, VertexIter vend,
                     data_structures::BiMap<typename IterToElem<VertexIter>::type> & idx) {
    typedef typename Metric::VertexType VertexType;
    idx = data_structures::BiMap<VertexType>(vbegin, vend); 
    MetricOnIdx<Metric> idxMetric(m, idx);
    return metricToBGL(idxMetric, vbegin, vend);
}

} //helpers
} //paal
