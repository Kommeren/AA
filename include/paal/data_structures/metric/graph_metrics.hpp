/**
 * @file graph_metrics.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */
#ifndef GRAPH_METRICS_HPP
#define GRAPH_METRICS_HPP

#include "basic_metrics.hpp"

#include <boost/graph/johnson_all_pairs_shortest.hpp>
#include <boost/graph/floyd_warshall_shortest.hpp>
#include <boost/graph/adjacency_matrix.hpp>

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
template <typename Graph> struct graph_metric_traits {
    typedef graph_type::Sparse GraphTypeTag;
};

/// implementation of different strategies of computing metric
namespace metric_fillers {

/// generic
template <typename GraphTypeTag> struct graph_metric_filler_impl;

/**
 * @brief specialization for Sparse graphs
 */
template <> struct graph_metric_filler_impl<graph_type::Sparse> {
    /**
     * @brief fill_matrix function
     *
     * @tparam Graph
     * @tparam ResultMatrix
     * @param g
     * @param rm
     */
    template <typename Graph, typename ResultMatrix>
    void fill_matrix(const Graph &g, ResultMatrix &rm) {
        boost::johnson_all_pairs_shortest_paths(g, rm);
    }
};

/**
 * @brief specialization for Dense graphs
 */
template <> struct graph_metric_filler_impl<graph_type::Dense> {
    template <typename Graph, typename ResultMatrix>
    /**
     * @brief fill_matrixFunction
     *
     * @param g
     * @param rm
     */
        void fill_matrix(const Graph &g, ResultMatrix &rm) {
        boost::floyd_warshall_all_pairs_shortest_paths(g, rm);
    }
};
}

/**
 * @class graph_metric
 * @brief Adopts boost graph as \ref metric.
 *
 * @tparam Graph
 * @tparam DistanceType
 * @tparam GraphType
 */
// GENERIC
// GraphType could be sparse, dense, large ...
template <
    typename Graph, typename DistanceType,
    typename GraphType = typename graph_metric_traits<Graph>::GraphTypeTag>
struct graph_metric : public array_metric<DistanceType>,
                      public metric_fillers::graph_metric_filler_impl<
                          typename graph_metric_traits<Graph>::GraphTypeTag> {
    typedef array_metric<DistanceType> GMBase;
    typedef metric_fillers::graph_metric_filler_impl<
        typename graph_metric_traits<Graph>::GraphTypeTag> GMFBase;

    /**
     * @brief constructor
     *
     * @param g
     */
    graph_metric(const Graph &g) : GMBase(num_vertices(g)) {
        GMFBase::fill_matrix(g, GMBase::m_matrix);
    }
};

// TODO implement
/// Specialization for large graphs
template <typename Graph, typename DistanceType>
struct graph_metric<Graph, DistanceType, graph_type::Large> {
    /**
     * @brief constructor
     *
     * @param g
     */
    graph_metric(const Graph &g) { assert(false); }
};

/// Specialization for adjacency_list
template <typename OutEdgeList, typename VertexList, typename Directed,
          typename VertexProperties, typename EdgeProperties,
          typename GraphProperties, typename EdgeList>
struct graph_metric_traits<
    boost::adjacency_list<OutEdgeList, VertexList, Directed, VertexProperties,
                          EdgeProperties, GraphProperties, EdgeList>> {
    typedef graph_type::Sparse GraphTypeTag;
};

/// Specialization for adjacency_matrix
template <typename Directed, typename VertexProperty, typename EdgeProperty,
          typename GraphProperty, typename Allocator>
struct graph_metric_traits<boost::adjacency_matrix<
    Directed, VertexProperty, EdgeProperty, GraphProperty, Allocator>> {
    typedef graph_type::Dense GraphTypeTag;
};

} //!data_structures
} //!paal

#endif // GRAPH_METRICS_HPP
