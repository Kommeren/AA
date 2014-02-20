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

/**
 * @brief traits for graph metric
 *
 * @tparam Graph
 */
template <typename Graph> struct GraphMetricTraits {
    typedef graph_type::Sparse GraphTypeTag;
};


///implementation of different strategies of computing metric
namespace metric_fillers {

    ///generic
    template <typename GraphTypeTag> struct GraphMetricFillerImpl;

    /**
     * @brief specialization for Sparse graphs
     */
    template <> struct GraphMetricFillerImpl<graph_type::Sparse> {
        /**
         * @brief fillMatrix function
         *
         * @tparam Graph
         * @tparam ResultMatrix
         * @param g
         * @param rm
         */
        template <typename Graph, typename ResultMatrix>
        void fillMatrix(const Graph & g, ResultMatrix & rm)  {
            boost::johnson_all_pairs_shortest_paths(g, rm);
        }
    };

    /**
     * @brief specialization for Dense graphs
     */
    template <> struct GraphMetricFillerImpl<graph_type::Dense> {
        template <typename Graph, typename ResultMatrix>
            /**
             * @brief fillMatrixFunction
             *
             * @param g
             * @param rm
             */
        void fillMatrix(const Graph & g, ResultMatrix & rm)  {
            boost::floyd_warshall_all_pairs_shortest_paths(g, rm);
        }
    };
}


/**
 * @class GraphMetric
 * @brief Adopts boost graph as \ref metric.
 *
 * @tparam Graph
 * @tparam DistanceType
 * @tparam GraphType
 */
// GENERIC
// GraphType could be sparse, dense, large ...
template <typename Graph, typename DistanceType,
          typename GraphType = typename GraphMetricTraits<Graph>::GraphTypeTag >
struct  GraphMetric : public ArrayMetric<DistanceType>,
            public metric_fillers::GraphMetricFillerImpl<typename GraphMetricTraits<Graph>::GraphTypeTag> {
      typedef   ArrayMetric<DistanceType> GMBase;
      typedef metric_fillers::GraphMetricFillerImpl<typename GraphMetricTraits<Graph>::GraphTypeTag> GMFBase;

      /**
       * @brief constructor
       *
       * @param g
       */
    GraphMetric(const Graph & g)
        : GMBase(num_vertices(g)) {
            GMFBase::fillMatrix(g, GMBase::m_matrix);
    }
};


//TODO implement
///Specialization for large graphs
template <typename Graph, typename DistanceType>
struct  GraphMetric<Graph, DistanceType, graph_type::Large> {
    /**
     * @brief constructor
     *
     * @param g
     */
    GraphMetric(const Graph & g) { assert(false);}
};

///Specialization for adjacency_list
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

///Specialization for adjacency_matrix
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


} //!data_structures
} //!paal

#endif //GRAPH_METRICS_HPP
