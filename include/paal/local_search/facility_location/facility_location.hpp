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
 * @brief this is model of LocalSearchStepMultiSolution concept. See \ref local_search.<br>
 * The Update is facility_location::Update. <br>
 * The Solution is adapted data_structures::FacilityLocationSolution. <br>
 * The SolutionElement is facility_location::Facility  <br>
 * Use DefaultFLComponents for default search components.
 *
 * The FacilityLocationLocalSearchStep takes as constructor parameter  data_structures::FacilityLocationSolution.
 * <b> WARNING </b>
 * getSolution of the FacilityLocationLocalSearchStep returns type ObjectWithCopy<FacilityLocationSolution>.
 * If you want to perform search, then change the solution object and continue local search you should perform all the operations on ObjectWithCopy. <br>
 * example: 
    \snippet facility_location_example.cpp FL Search Example
 *
 * full example is facility_location_example.cpp
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
            FLSolution fls,
            MultiSearchComponents sc = MultiSearchComponents()) :
                base(FLSolutionAdapter(std::move(fls)), 
                                       std::move(sc)) {}
};

};
};
};
