
#ifndef __2_LOCAL_SEARCH__
#define __2_LOCAL_SEARCH__

#include "local_search/local_search.hpp"
#include "local_search/trivial_neighbour.hpp"
#include "local_search/2_local_search/2_local_search_swapper.hpp"
#include "local_search/2_local_search/2_opt_checker.hpp"
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
                CheckIfImprove, TwoLocalSearchSwapper<CycleManager> >  {

                typedef LocalSearchStep<typename CycleManager::EdgeIterator, NeighbourGetter, 
                    CheckIfImprove, TwoLocalSearchSwapper<CycleManager> > LocalSearchStepT;

                 public:

                     template <typename SolutionIter>  TwoLocalSearchStep(SolutionIter begin, SolutionIter end, NeighbourGetter && ng = TrivialNeigbourGetter(), 
                             CheckIfImprove && check = CheckIfImprove2Opt<Metric>()) 
                         : LocalSearchStepT(begin, end, ng, check, 
                                 TwoLocalSearchSwapper<CycleManager>(CycleManager(begin, end))) {}
             };

} //two_local_search
} //local_search
} //paal

#endif // __2_LOCAL_SEARCH__

