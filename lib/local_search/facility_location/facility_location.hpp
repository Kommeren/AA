#include <set>
#include "facility_location_update_element.hpp"
#include "facility_location_solution_adapter.hpp"
#include "data_structures/facility_location_solution.hpp"
#include "facility_location_neighbour_getter.hpp"
#include "facility_location_checker.hpp"
#include "facility_location_updater.hpp"
#include "local_search/local_search_multi_solution.hpp"

namespace paal {
namespace local_search {
namespace facility_location {
    

template <typename FacilityLocationSolution,
          typename NeighbourhoodGetter = FacilityLocationNeighbourhoodGetter<typename FacilityLocationSolution::VertexType>,
          typename ImproveChecker      = FacilityLocationChecker            <typename FacilityLocationSolution::VertexType>,
          typename Updater             = FacilityLocationUpdater            <typename FacilityLocationSolution::VertexType>>

class FacilityLocationLocalSearchStep : 
    public LocalSearchStepMultiSolution<
               FacilityLocationSolutionAdapter<FacilityLocationSolution>, 
               NeighbourhoodGetter, 
               ImproveChecker, 
               Updater>  {

public:
    typedef FacilityLocationSolutionAdapter<FacilityLocationSolution> FLSolutionAdapter;
    
    typedef LocalSearchStepMultiSolution<
                FLSolutionAdapter,
                NeighbourhoodGetter, 
                ImproveChecker, 
                Updater >  base;

        FacilityLocationLocalSearchStep(FacilityLocationSolution fls, 
                                    NeighbourhoodGetter ng = NeighbourhoodGetter(),
                                    ImproveChecker ch = ImproveChecker(),
                                    Updater u = Updater()) :

                                        base(FLSolutionAdapter(std::move(fls)), std::move(ng), 
                                                               std::move(ch), std::move(u)) {}
};

};
};
};
