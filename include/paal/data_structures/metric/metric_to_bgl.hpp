#include <boost/graph/adjacency_matrix.hpp>

#include "paal/data_structures/bimap.hpp"
#include "paal/data_structures/metric/metric_traits.hpp"
#include "paal/data_structures/metric/metric_on_idx.hpp"

namespace paal {
namespace data_structures {
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
    typedef MetricTraits<Metric> MT;
    typedef typename MT::VertexType VertexType;
    typedef typename MT::DistanceType Dist;
    Graph g(N);
    auto r = boost::make_iterator_range(vbegin, vend);
    for(VertexType v : r){
        for(VertexType w : r){
            if(v < w) {
                bool succ = boost::add_edge(v, w, 
                        boost::property<boost::edge_weight_t, Dist>(m(v,w)), g).second;
                assert(succ);
            }
        }
    }
    return g;
}
 
template <typename Metric, typename VertexIter>                
 typename   AdjacencyMatrix<Metric>::type
metricToBGLWithIndex(const Metric & m, VertexIter vbegin, VertexIter vend,
                     BiMap<typename std::iterator_traits<VertexIter>::value_type> & idx) {
    typedef data_structures::MetricTraits<Metric> MT;
    typedef typename MT::VertexType VertexType;
    idx = data_structures::BiMap<VertexType>(vbegin, vend); 
    auto  idxMetric = data_structures::make_metricOnIdx(m, idx);
    std::function<int(VertexType)> trans = [&](VertexType v) {return idx.getIdx(v);};
    return metricToBGL(idxMetric, boost::make_transform_iterator(vbegin, trans), 
                                  boost::make_transform_iterator(vend, trans));
}

} //data_structures
} //paal
