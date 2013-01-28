
#ifndef __2_LOCAL_SEARCH__
#define __2_LOCAL_SEARCH__

#include "local_search/local_search_multi_solution.hpp"
#include "local_search/trivial_neighbour.hpp"
#include "local_search/2_local_search/2_local_search_updater.hpp"
#include "local_search/2_local_search/2_local_search_checker.hpp"
#include "local_search/2_local_search/2_local_search_solution_adapter.hpp"
#include "data_structures/simple_cycle_manager.hpp"

namespace paal {
namespace local_search {
namespace two_local_search {

template <typename VertexType, 
          typename Metric, 
          typename NeighbourGetter = TrivialNeigbourGetter,
          template <class> class CheckIfImprove = CheckIfImprove2Opt,
          template <class> class Cycle = data_structures::SimpleCycle>

         class  TwoLocalSearchStep : 
             public LocalSearchStepMultiSolution<TwoLocalSearchContainer<Cycle<VertexType>>, 
                        NeighbourGetter, CheckIfImprove<Metric>, TwoLocalSearchUpdater >  {

               
                typedef Cycle<VertexType> CycleT;
                typedef LocalSearchStepMultiSolution<TwoLocalSearchContainer<CycleT> , NeighbourGetter, 
                    CheckIfImprove<Metric>, TwoLocalSearchUpdater > LocalSearchStepT;

                public:

                    template <typename SolutionIter>  
                     TwoLocalSearchStep(SolutionIter solBegin, SolutionIter solEnd, 
                                        Metric & m, 
                                        NeighbourGetter ng = NeighbourGetter()) 

                        :      LocalSearchStepT(TwoLocalSearchContainer<CycleT>(m_cycle), std::move(ng), 
                                 CheckIfImprove<Metric>(m), TwoLocalSearchUpdater()),
                                m_cycle(solBegin, solEnd) {}

                    CycleT & getCycle() {
                        return m_cycle;
                    }

                private:
                    CycleT m_cycle;
             };

} //two_local_search
} //local_search
} //paal

#endif // __2_LOCAL_SEARCH__

