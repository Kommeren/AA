/**
 * @file metric_traits.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-03-04
 */
#ifndef METRIC_TRAITS_HPP
#define METRIC_TRAITS_HPP
#include <type_traits>

#include "paal/utils/type_functions.hpp"

namespace paal {
namespace data_structures {

    /**
     * @brief base for metric traits
     *
     * @tparam Metric
     * @tparam _VertexType
     */
template <typename Metric, typename _VertexType>
struct _MetricTraits {
    typedef int VertexType;
    ///Distance type
    typedef puretype(std::declval<Metric>()
             (std::declval<VertexType>(), std::declval<VertexType>())) DistanceType;
};

/**
 * @brief metric traits
 *
 * @tparam Metric
 */
template <typename Metric>
struct MetricTraits : public _MetricTraits<Metric, int> {};

}
}
#endif /* METRIC_TRAITS_HPP */
