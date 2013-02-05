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
#include "data_structures/facility_location_solution.hpp"
#include "facility_location_neighbor_getter.hpp"
#include "facility_location_checker.hpp"
#include "facility_location_updater.hpp"
#include "local_search/local_search_multi_solution.hpp"

namespace paal {
namespace local_search {
namespace facility_location {
    

template <typename Voronoi,
          typename FacilityCost,
          typename NeighborhoodGetter = FacilityLocationNeighborhoodGetter<typename Voronoi::VertexType>,
          typename ImproveChecker      = FacilityLocationChecker          <typename Voronoi::VertexType>,
          typename Updater             = FacilityLocationUpdater          <typename Voronoi::VertexType>>

class FacilityLocationLocalSearchStep : 
    public LocalSearchStepMultiSolution<
               FacilityLocationSolutionAdapter<
                    data_structures::FacilityLocationSolution<FacilityCost, Voronoi>>, 
               NeighborhoodGetter, 
               ImproveChecker, 
               Updater>  {

public:
    typedef data_structures::FacilityLocationSolution<FacilityCost, Voronoi> FLSolution;
    typedef FacilityLocationSolutionAdapter<FLSolution> FLSolutionAdapter;
    
    typedef LocalSearchStepMultiSolution<
                FLSolutionAdapter,
                NeighborhoodGetter, 
                ImproveChecker, 
                Updater >  base;

        FacilityLocationLocalSearchStep(Voronoi voronoi,
                                        FacilityCost cost,
                                        typename FLSolution::UnchosenFacilitiesSet uch,
                                        NeighborhoodGetter ng = NeighborhoodGetter(),
                                        ImproveChecker ch = ImproveChecker(),
                                        Updater u = Updater()) :

                                        base(FLSolutionAdapter(FLSolution(std::move(voronoi), std::move(uch), std::move(cost))), 
                                                                std::move(ng), std::move(ch), std::move(u)) {}
};

};
};
};
