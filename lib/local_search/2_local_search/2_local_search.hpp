
#ifndef __2_LOCAL_SEARCH__
#define __2_LOCAL_SEARCH__

#include "local_search/local_search.hpp"
#include "local_search/trivial_neighbour.hpp"
#include "local_search/2_local_search/2_local_search_updater.hpp"
#include "local_search/2_local_search/2_local_search_checker.hpp"
#include "data_structures/simple_cycle_manager.hpp"

namespace paal {
namespace local_search {
namespace two_local_search {


template <typename SolutionElement, 
          typename Metric, 
          typename NeighbourGetter = TrivialNeigbourGetter,
          typename CheckIfImprove = CheckIfImprove2Opt<Metric> ,
          typename CycleManager = data_structures::SimpleCycleManager<SolutionElement> >

         class  TwoLocalSearchStep : 
             public LocalSearchStep<typename CycleManager::EdgeIterator, NeighbourGetter, 
                CheckIfImprove, TwoLocalSearchUpdater<CycleManager> >  {

                typedef LocalSearchStep<typename CycleManager::EdgeIterator, NeighbourGetter, 
                    CheckIfImprove, TwoLocalSearchUpdater<CycleManager> > LocalSearchStepT;

                 public:

                     template <typename Solution>  TwoLocalSearchStep(Solution && sol, NeighbourGetter && ng = TrivialNeigbourGetter(), 
                             CheckIfImprove && check = CheckIfImprove2Opt<Metric>()) 
                         : LocalSearchStepT(sol, ng, check, 
                                 TwoLocalSearchUpdater<CycleManager>(CycleManager(sol.begin(), sol.end()))) {}
             };

} //two_local_search
} //local_search
} //paal

#endif // __2_LOCAL_SEARCH__

