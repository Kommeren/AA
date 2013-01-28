
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
          typename NeighbourhoodGetter = TrivialNeigbourGetter,
          template <class> class ImproveChecker = ImproveChecker2Opt,
          template <class> class Cycle = data_structures::SimpleCycle>

         class  TwoLocalSearchStep : 
             public LocalSearchStepMultiSolution<TwoLocalSearchContainer<Cycle<VertexType>>, 
                        NeighbourhoodGetter, ImproveChecker<Metric>, TwoLocalSearchUpdater >  {

               
                typedef Cycle<VertexType> CycleT;
                typedef LocalSearchStepMultiSolution<TwoLocalSearchContainer<CycleT> , NeighbourhoodGetter, 
                    ImproveChecker<Metric>, TwoLocalSearchUpdater > LocalSearchStepT;

                public:

                    template <typename SolutionIter>  
                     TwoLocalSearchStep(SolutionIter solBegin, SolutionIter solEnd, 
                                        Metric & m, 
                                        ImproveChecker<Metric> ich,
                                        NeighbourhoodGetter ng = NeighbourhoodGetter()
                                        ) 

                        : LocalSearchStepT(TwoLocalSearchContainer<CycleT>(m_cycle), std::move(ng), 
                                                std::move(ich), TwoLocalSearchUpdater()),
                                                m_cycle(solBegin, solEnd) {}
                    
                     TwoLocalSearchStep(Cycle<VertexType> c, 
                                        Metric & m, 
                                        ImproveChecker<Metric> ich,
                                        NeighbourhoodGetter ng = NeighbourhoodGetter()
                                        ) 

                        :   LocalSearchStepT(TwoLocalSearchContainer<CycleT>(m_cycle), std::move(ng), 
                                                std::move(ich), TwoLocalSearchUpdater()),
                                                m_cycle(std::move(c)) {}

                private:
                    CycleT m_cycle;
             };

} //two_local_search
} //local_search
} //paal

#endif // __2_LOCAL_SEARCH__

