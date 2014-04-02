/**
 * @file metric_to_bgl.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */
#define BOOST_RESULT_OF_USE_DECLTYPE


#include "paal/data_structures/bimap.hpp"
#include "paal/data_structures/metric/metric_traits.hpp"
#include "paal/data_structures/metric/metric_on_idx.hpp"
#include "paal/utils/functors.hpp"

#include <boost/graph/adjacency_matrix.hpp>
#include <boost/iterator/transform_iterator.hpp>


namespace paal {
namespace data_structures {
//TODO it would be nice to adapt Matrix + something to bgl

    /**
     * @brief type of adjacency_matrix, for given metric
     *
     * @tparam Metric
     */
template <typename Metric>  struct adjacency_matrix {
    typedef data_structures::metric_traits<Metric> MT;
    typedef boost::adjacency_matrix<boost::undirectedS, boost::no_property,
                boost::property<boost::edge_weight_t, typename MT::DistanceType> > type;
};


 /**
  * @brief we assume that (vbegin, vend) is sequence of values  (0, vend - vbegin).
  *
  * @param m
  * @param vbegin
  * @param vend
  */
template <typename Metric, typename VertexIter>
typename   adjacency_matrix<Metric>::type
metric_to_bgl( const Metric & m, VertexIter vbegin, VertexIter vend) {
    typedef typename adjacency_matrix<Metric>::type Graph;
    const unsigned N = std::distance(vbegin, vend);
    typedef metric_traits<Metric> MT;
    typedef typename MT::VertexType VertexType;
    typedef typename MT::DistanceType Dist;
    Graph g(N);
    auto r = boost::make_iterator_range(vbegin, vend);
    for(VertexType v : r){
        for(VertexType w : r){
            if(v < w) {
                bool succ = add_edge(v, w,
                        boost::property<boost::edge_weight_t, Dist>(m(v,w)), g).second;
                assert(succ);
            }
        }
    }
    return g;
}

/**
 * @brief  produces graph from metric with index
 *
 * @tparam Metric
 * @tparam VertexIter
 * @param m
 * @param vbegin
 * @param vend
 * @param idx
 *
 * @return
 */
template <typename Metric, typename VertexIter>
 typename   adjacency_matrix<Metric>::type
metric_to_bgl_with_index(const Metric & m, VertexIter vbegin, VertexIter vend,
                     bimap<typename std::iterator_traits<VertexIter>::value_type> & idx) {
    typedef data_structures::metric_traits<Metric> MT;
    typedef typename MT::VertexType VertexType;
    idx = data_structures::bimap<VertexType>(vbegin, vend);
    auto  idxMetric = data_structures::make_metric_on_idx(m, idx);
    auto transLambda = [&](VertexType v) {return idx.get_idx(v);};
    auto trans = utils::make_assignable_functor(transLambda);
    return metric_to_bgl(idxMetric, boost::make_transform_iterator(vbegin, trans),
                                  boost::make_transform_iterator(vend, trans));
}

} //!data_structures
} //!paal
