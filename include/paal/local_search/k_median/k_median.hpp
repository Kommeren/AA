/**
 * @file k_median.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-03-08
 */
#ifndef K_MEDIAN_HPP
#define K_MEDIAN_HPP

#include "paal/local_search/multi_solution/local_search_multi_solution.hpp"
#include "paal/local_search/facility_location/facility_location_swap.hpp"
#include "paal/local_search/facility_location/facility_location_solution_adapter.hpp"
#include "paal/local_search/facility_location/facility_location.hpp"

#include "paal/data_structures/facility_location/k_median_solution.hpp"

namespace paal {
namespace local_search {
namespace k_median {

/**
 * @class DefaultKMedianComponents
 * @brief Model of MultiSearchComponents with default multi search components for k-median.
 *
 * @tparam VertexType
 */
template <typename VertexType>
struct DefaultKMedianComponents {
    typedef MultiSearchComponents<
                facility_location::FacilityLocationGetMovesSwap <VertexType>,
                facility_location::FacilityLocationGainSwap         <VertexType>,
                facility_location::FacilityLocationCommitSwap         <VertexType>> type;
};

}
}
}

#endif /* K_MEDIAN_HPP */
