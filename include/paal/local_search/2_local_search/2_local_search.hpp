/**
 * @file 2_local_search.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */

#ifndef __2_LOCAL_SEARCH__
#define __2_LOCAL_SEARCH__

#include "local_search/local_search_multi_solution.hpp"
#include "local_search/trivial_neighbor.hpp"
#include "local_search/2_local_search/2_local_search_updater.hpp"
#include "local_search/2_local_search/2_local_search_checker.hpp"
#include "local_search/2_local_search/2_local_search_solution_adapter.hpp"
#include "data_structures/simple_cycle_manager.hpp"

namespace paal {
namespace local_search {
namespace two_local_search {


template <typename Cycle,
          typename ImproveChecker, 
          typename NeighborhoodGetter = TrivialNeigborGetter, 
          typename StopCondition = TrivialStopConditionMultiSolution>

         class  TwoLocalSearchStep : 
             public LocalSearchStepMultiSolution<TwoLocalSearchContainer<Cycle>, 
                        NeighborhoodGetter, ImproveChecker, TwoLocalSearchUpdater, StopCondition>  {
               
                typedef LocalSearchStepMultiSolution<TwoLocalSearchContainer<Cycle> , NeighborhoodGetter, 
                            ImproveChecker, TwoLocalSearchUpdater, StopCondition > LocalSearchStepT;

                public:

                     TwoLocalSearchStep(Cycle c, 
                                        ImproveChecker ich, 
                                        NeighborhoodGetter ng = NeighborhoodGetter(),
                                        StopCondition sc = TrivialStopConditionMultiSolution()) 

                        :   LocalSearchStepT(TwoLocalSearchContainer<Cycle>(m_cycle), std::move(ng), 
                                                std::move(ich), TwoLocalSearchUpdater(), std::move(sc)),
                                                m_cycle(std::move(c)) {}

                private:
                    Cycle m_cycle;
             };



/**
 * @brief make template function for TwoLocalSearchStep, just to avoid providing type names in template.
 *
 *
 * @tparam Cycle
 * @tparam ImproveChecker
 * @tparam NeighborhoodGetter
 * @param c
 * @param ich
 * @param ng
 *
 * @return 
 */
template <typename Cycle,
          typename ImproveChecker, 
          typename NeighborhoodGetter = TrivialNeigborGetter,
          typename StopCondition = TrivialStopConditionMultiSolution>

TwoLocalSearchStep<Cycle, ImproveChecker, NeighborhoodGetter, StopCondition>  

    make_TwoLocalSearchStep(Cycle c, ImproveChecker ich, 
            NeighborhoodGetter ng = TrivialNeigborGetter(),
            StopCondition sc = TrivialStopConditionMultiSolution()) {

    return TwoLocalSearchStep<Cycle, ImproveChecker, NeighborhoodGetter>(c, ich, ng, sc);
}


} //two_local_search
} //local_search
} //paal

#endif // __2_LOCAL_SEARCH__

