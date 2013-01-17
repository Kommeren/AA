
#ifndef __2_LOCAL_SEARCH__
#define __2_LOCAL_SEARCH__

#include "local_search/local_search.hpp"
#include "local_search/trivial_neighbour.hpp"
#include "local_search/2_local_search/2_local_search_updater.hpp"
#include "local_search/2_local_search/2_local_search_checker.hpp"
#include "local_search/2_local_search/2_local_search_solution_container.hpp"
#include "data_structures/simple_cycle_manager.hpp"

namespace paal {
namespace local_search {
namespace two_local_search {


template <typename VertexType, 
          typename Metric, 
          typename CheckIfImprove = CheckIfImprove2Opt<Metric> ,
          typename CycleManager = data_structures::SimpleCycleManager<VertexType>,
          typename NeighbourGetter = TrivialNeigbourGetter>

         class  TwoLocalSearchStep : 
             public LocalSearchStepMultiSolution<TwoLocalSearchContainer<CycleManager>, 
                        NeighbourGetter, CheckIfImprove, TwoLocalSearchUpdater >  {

                typedef LocalSearchStepMultiSolution<TwoLocalSearchContainer<CycleManager> , NeighbourGetter, 
                    CheckIfImprove, TwoLocalSearchUpdater > LocalSearchStepT;

                public:

                    //TODO INAPPROPRIATE ORDER  could be invalid
                    template <typename SolutionIter>  
                     TwoLocalSearchStep(SolutionIter solBegin, SolutionIter solEnd, Metric & m
                             , NeighbourGetter ng = NeighbourGetter()) 
                        :      LocalSearchStepT(m_cycleAdapter, ng, 
                                 CheckIfImprove(m), TwoLocalSearchUpdater()),
                                m_cycleManager(solBegin, solEnd),
                                m_cycleAdapter(m_cycleManager) {}

                    CycleManager & getCycle() {
                        return m_cycleManager;
                    }

                private:
                    CycleManager  m_cycleManager;
                    TwoLocalSearchContainer<CycleManager> m_cycleAdapter;
             };

} //two_local_search
} //local_search
} //paal

#endif // __2_LOCAL_SEARCH__

