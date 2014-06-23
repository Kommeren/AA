/**
 * @file voronoi_traits.hpp
 * @brief voronoi traits
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-03-06
 */
#ifndef VORONOI_TRAITS_HPP
#define VORONOI_TRAITS_HPP

#include "paal/utils/type_functions.hpp"

namespace paal {
namespace data_structures {

/**
 * @brief voronoi traits base
 *
 * @tparam V
 * @tparam Vertex
 */
template <typename V, typename Vertex> struct _voronoi_traits {
    typedef Vertex VertexType;
    /// distance type
    typedef decltype(std::declval<V>().add_generator(
        std::declval<VertexType>())) DistanceType;

    /// Generators set
    typedef puretype(std::declval<V>().get_generators()) GeneratorsSet;

    /// vertices set
    typedef puretype(std::declval<V>().get_vertices()) VerticesSet;
};

/// default VertexType is int.
template <typename V> struct voronoi_traits : public _voronoi_traits<V, int> {};
}
}
#endif /* VORONOI_TRAITS_HPP */
