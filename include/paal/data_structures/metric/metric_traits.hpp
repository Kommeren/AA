/**
 * @file metric_traits.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-03-04
 */
#ifndef METRIC_TRAITS_HPP
#define METRIC_TRAITS_HPP

#include "paal/utils/type_functions.hpp"

#include <type_traits>

namespace paal {
namespace data_structures {

/**
 * @brief base for metric traits
 *
 * @tparam Metric
 * @tparam _VertexType
 */
template <typename Metric, typename _VertexType> struct _metric_traits {
    typedef _VertexType VertexType;
    /// Distance type
    typedef puretype(std::declval<Metric>()(
        std::declval<VertexType>(), std::declval<VertexType>())) DistanceType;
};

/**
 * @brief metric traits
 *
 * @tparam Metric
 */
template <typename Metric>
struct metric_traits : public _metric_traits<Metric, int> {};
}
}
#endif /* METRIC_TRAITS_HPP */
