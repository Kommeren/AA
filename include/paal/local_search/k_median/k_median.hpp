/**
 * @file k_median.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-03-08
 */
#ifndef K_MEDIAN_HPP
#define K_MEDIAN_HPP

#include "paal/local_search/facility_location/facility_location_swap.hpp"
#include "paal/local_search/facility_location/facility_location_solution_adapter.hpp"
#include "paal/local_search/facility_location/facility_location.hpp"

#include "paal/data_structures/facility_location/k_median_solution.hpp"

namespace paal {
namespace local_search {
namespace k_median {

/**
 * @class default_k_median_components
 * @brief Model of Multisearch_components with default multi search components for k-median.
 */
struct default_k_median_components {
    typedef Multisearch_components<
                facility_location::facility_locationget_moves_swap ,
                facility_location::facility_location_gain_swap     ,
                facility_location::facility_location_commit_swap   > type;
};

}
}
}

#endif /* K_MEDIAN_HPP */
