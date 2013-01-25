#include <set>
#include "facility_location_update_element.hpp"
#include "facility_location_solution_adapter.hpp"
#include "data_structures/facility_location_solution.hpp"
#include "facility_location_neighbour_getter.hpp"
#include "facility_location_checker.hpp"
#include "facility_location_updater.hpp"
#include "local_search/local_search.hpp"

namespace paal {
namespace local_search {
namespace facility_location {
    

template <typename VertexType,
         typename Metric, 
         typename FacilityCosts,
          template <class> class NeighbourGetter = FacilityLocationNeighbourGetter,
          template <class> class CheckIfImprove = FacilityLocationChecker,
          template <class> class Update = FacilityLocationUpdater,
          template <class,class,class> class FacilityLocationSolution 
              = data_structures::FacilityLocationSolutionWithClientsAssignment>
class FacilityLocationLocalSearchStep : 
    public LocalSearchStepMultiSolution<
            FacilityLocationSolutionAdapter<
               FacilityLocationSolution<VertexType, 
                                        Metric, 
                                        FacilityCosts>
                                       >, 
            NeighbourGetter<VertexType>, 
            CheckIfImprove<VertexType>, 
            Update<VertexType> >  {
    
    typedef FacilityLocationSolution<VertexType, 
                                     Metric, 
                                     FacilityCosts>
                                       FLSolution;
    
    typedef FacilityLocationSolutionAdapter<
               FLSolution> FLSolutionAdapter;
    
    typedef LocalSearchStepMultiSolution<
            FLSolutionAdapter,
            NeighbourGetter<VertexType>, 
            CheckIfImprove<VertexType>, 
            Update<VertexType> >  base;

    template <typename ChosenCol, typename UnchosenCol, typename ClientsCol> 
        FacilityLocationLocalSearchStep(ChosenCol  chosenFacilities, UnchosenCol  unchosenFacilities, 
                                    ClientsCol  clients, FacilityCosts & facilitiesCosts, Metric & m, 
                                    NeighbourGetter<VertexType> ng = NeighbourGetter<VertexType>(),
                                    CheckIfImprove<VertexType> ch = CheckIfImprove<VertexType>(),
                                    Update<VertexType> u = Update<VertexType>()) :

                                        base(FLSolutionAdapter(m_fls), std::move(ng), std::move(ch), std::move(u)),
                                        m_fls(std::move(unchosenFacilities), 
                                              std::move(chosenFacilities), 
                                              std::move(clients), 
                                              m, facilitiesCosts) {}
            

    private:
        FLSolution  m_fls;
};

};
};
};
