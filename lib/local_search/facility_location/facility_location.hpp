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
    

template <typename VertexType,
         typename Metric, 
         typename FacilityCosts,
          template <class> class NeighbourhoodGetter = FacilityLocationNeighbourhoodGetter,
          template <class> class ImproveChecker = FacilityLocationChecker,
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
            NeighbourhoodGetter<VertexType>, 
            ImproveChecker<VertexType>, 
            Update<VertexType> >  {
    
    typedef FacilityLocationSolution<VertexType, 
                                     Metric, 
                                     FacilityCosts>
                                       FLSolution;
    
    typedef FacilityLocationSolutionAdapter<
               FLSolution> FLSolutionAdapter;
    
    typedef LocalSearchStepMultiSolution<
            FLSolutionAdapter,
            NeighbourhoodGetter<VertexType>, 
            ImproveChecker<VertexType>, 
            Update<VertexType> >  base;

    template <typename ChosenCol, typename UnchosenCol, typename ClientsCol> 
        FacilityLocationLocalSearchStep(ChosenCol  chosenFacilities, UnchosenCol  unchosenFacilities, 
                                    ClientsCol  clients, FacilityCosts & facilitiesCosts, Metric & m, 
                                    NeighbourhoodGetter<VertexType> ng = NeighbourhoodGetter<VertexType>(),
                                    ImproveChecker<VertexType> ch = ImproveChecker<VertexType>(),
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
