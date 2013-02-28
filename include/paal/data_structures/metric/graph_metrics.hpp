/**
 * @file graph_metrics.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */
#ifndef GRAPH_METRICS_HPP
#define GRAPH_METRICS_HPP

#include <boost/graph/johnson_all_pairs_shortest.hpp>
#include <boost/graph/floyd_warshall_shortest.hpp>
#include <boost/graph/adjacency_matrix.hpp>

#include "basic_metrics.hpp"

namespace paal {
namespace data_structures {

namespace graph_type {
    class Sparse;
    class Dense;
    class Large;
}

template <typename Graph> struct GraphMetricTraits { 
    typedef graph_type::Sparse GraphTypeTag;
};


//impplementation of different startegies of computing metric
namespace metric_fillers {

    //generic
    template <typename GraphTypeTag> class GraphMericFillerImpl {};
    
    template <> class GraphMericFillerImpl<graph_type::Sparse> {
        public:
        template <typename Graph, typename ResultMatrics> 
        void fillMatrix(const Graph & g, ResultMatrics & rm)  {
	    boost::johnson_all_pairs_shortest_paths(g, rm); 
        }
    };
    
    template <> class GraphMericFillerImpl<graph_type::Dense> {
        public:
        template <typename Graph, typename ResultMatrics> 
        void fillMatrix(const Graph & g, ResultMatrics & rm)  {
	    boost::floyd_warshall_initialized_all_pairs_shortest_paths(g, rm);
        }
    };
}


// GENERIC
// GraphType could be sparse, dense, large ...
template <typename Graph, typename DistanceType, 
          typename GraphType = typename GraphMetricTraits<Graph>::GraphTypeTag > 
struct  GraphMetric : public ArrayMetric<DistanceType>, 
            public metric_fillers::GraphMericFillerImpl<typename GraphMetricTraits<Graph>::GraphTypeTag> {
      typedef   ArrayMetric<DistanceType> GMBase;

    GraphMetric(const Graph & g)  
        : GMBase(boost::num_vertices(g)) {
        fillMatrix(g, GMBase::m_matrix);
    }
};


//Specialization for large graphs
//TODO implement
template <typename Graph, typename DistanceType> 
struct  GraphMetric<Graph, DistanceType, graph_type::Large> {
    GraphMetric(const Graph & g) { assert(false);} 
};

//Specialization for adjacency_list
template <typename OutEdgeList, 
          typename VertexList, 
          typename Directed,
          typename VertexProperties, 
          typename EdgeProperties,
          typename GraphProperties, 
          typename EdgeList>
struct GraphMetricTraits<boost::adjacency_list<
                             OutEdgeList, 
                             VertexList, 
                             Directed,
                             VertexProperties, 
                             EdgeProperties,
                             GraphProperties, 
                             EdgeList>> {
                               typedef graph_type::Sparse GraphTypeTag;
};

//Specialization for adjacency_matrix
template <typename Directed, 
          typename VertexProperty, 
          typename EdgeProperty, 
          typename GraphProperty,
          typename Allocator>
struct GraphMetricTraits<boost::adjacency_matrix<
                            Directed, 
                            VertexProperty, 
                            EdgeProperty, 
                            GraphProperty,
                            Allocator>> {
                                typedef graph_type::Dense GraphTypeTag;
};


} //data_structures
} //paal

#endif //GRAPH_METRICS_HPP