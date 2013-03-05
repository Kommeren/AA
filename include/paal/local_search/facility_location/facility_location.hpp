/**
 * @file facility_location.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */
#include <set>
#include "facility_location_update_element.hpp"
#include "facility_location_solution_adapter.hpp"
#include "paal/data_structures/facility_location/facility_location_solution.hpp"
#include "facility_location_neighbor_getter.hpp"
#include "facility_location_checker.hpp"
#include "facility_location_updater.hpp"
#include "paal/local_search/multi_solution_step/local_search_multi_solution.hpp"

namespace paal {
namespace local_search {
namespace facility_location {

/**
 * @class DefaultFLComponents 
 * @brief Model of MultiSearchComponents with default multi search components for facility location.
 *
 * @tparam VertexType
 */
template <typename VertexType> 
struct DefaultFLComponents {
    typedef MultiSearchComponents<
                FacilityLocationGetNeighborhood<VertexType>,
                FacilityLocationChecker        <VertexType>,
                FacilityLocationUpdater        <VertexType>> type;
};

/**
 * @class FacilityLocationLocalSearchStep
 * @brief this is model of LocalSearchStepMultiSolution concept.
 * Use DefaultFLComponents for default search components.
 * TODO describe return type
 * example: 
    \snippet facility_location_example.cpp FL Search Example
 *
 * @tparam Voronoi
 * @tparam FacilityCost
 * @tparam MultiSearchComponents
 */
template <typename Voronoi,
          typename FacilityCost,
          typename MultiSearchComponents = typename DefaultFLComponents<typename Voronoi::VertexType>::type>

class FacilityLocationLocalSearchStep : 
    public LocalSearchStepMultiSolution<
               FacilityLocationSolutionAdapter<
                    data_structures::FacilityLocationSolution<FacilityCost, Voronoi>>, 
               MultiSearchComponents>  {

public:
    typedef data_structures::FacilityLocationSolution<FacilityCost, Voronoi> FLSolution;
    typedef FacilityLocationSolutionAdapter<FLSolution> FLSolutionAdapter;
    
    typedef LocalSearchStepMultiSolution<
                FLSolutionAdapter,
                MultiSearchComponents>  base;

    FacilityLocationLocalSearchStep(
            Voronoi voronoi,
            const FacilityCost & cost,
            typename FLSolution::UnchosenFacilitiesSet uch,
            MultiSearchComponents sc = MultiSearchComponents()) :

                base(FLSolutionAdapter(FLSolution(std::move(voronoi), std::move(uch), cost)), 
                                       std::move(sc)) {}
};

};
};
};
