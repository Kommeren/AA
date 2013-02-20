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
#include "paal/data_structures/facility_location_solution.hpp"
#include "facility_location_neighbor_getter.hpp"
#include "facility_location_checker.hpp"
#include "facility_location_updater.hpp"
#include "paal/local_search/multi_solution/local_search_multi_solution.hpp"

namespace paal {
namespace local_search {
namespace facility_location {

template <typename VertexType> 
struct DefaultFLComponents {
    typedef MultiSearchComponents<
                FacilityLocationNeighborhoodGetter<VertexType>,
                FacilityLocationChecker           <VertexType>,
                FacilityLocationUpdater           <VertexType>> type;
};
    

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
