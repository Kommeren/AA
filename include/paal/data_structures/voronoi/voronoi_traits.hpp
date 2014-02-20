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
template <typename V, typename Vertex>
struct _VoronoiTraits {
    typedef Vertex VertexType;
    ///distance type
    typedef decltype(std::declval<V>().addGenerator(std::declval<VertexType>()))
                DistanceType;

    /// Generators set
    typedef puretype(std::declval<V>().getGenerators())
                GeneratorsSet;

    ///vertices set
    typedef puretype(std::declval<V>().getVertices())
                VerticesSet;
};

///default VertexType is int.
template <typename V>
struct VoronoiTraits : public _VoronoiTraits<V, int> {
};

}
}
#endif /* VORONOI_TRAITS_HPP */
