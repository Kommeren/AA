/**
 * @file 2_local_search.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */

#ifndef TWO_LOCAL_SEARCH_HPP
#define TWO_LOCAL_SEARCH_HPP

#include "paal/local_search/multi_solution/trivial_neighbor.hpp"
#include "paal/local_search/multi_solution/trivial_stop_condition_multi_solution.hpp"
#include "paal/local_search/search_components.hpp"
#include "paal/local_search/multi_solution/local_search_multi_solution.hpp"
#include "paal/local_search/2_local_search/2_local_search_updater.hpp"
#include "paal/local_search/2_local_search/2_local_search_checker.hpp"
#include "paal/local_search/2_local_search/2_local_search_solution_adapter.hpp"
#include "paal/data_structures/cycle/cycle_start_from_last_change.hpp"

namespace paal {
namespace local_search {
namespace two_local_search {

template <typename Gain, 
          typename GetNeighborhood = TrivialNeigborGetter, 
          typename StopCondition = TrivialStopConditionMultiSolution> 
class TwoLocalComponents : 
    public MultiSearchComponents<
            GetNeighborhood, 
            Gain, 
            TwoLocalSearchUpdater, 
            StopCondition> {

    typedef MultiSearchComponents<
            GetNeighborhood, 
            Gain, 
            TwoLocalSearchUpdater, 
            StopCondition> base; 
public : 
    TwoLocalComponents(Gain ic = Gain(), 
                       GetNeighborhood ng = GetNeighborhood(), 
                       StopCondition sc = StopCondition()) : 
            base(std::move(ng), std::move(ic), TwoLocalSearchUpdater(), std::move(sc)) {}
};


//TODO check cycle concept
template <typename Cycle,
          typename SearchComponents>

class  TwoLocalSearchStep : 
   public LocalSearchStepMultiSolution<TwoLocalSearchAdapter<data_structures::CycleStartFromLastChange<Cycle>>, SearchComponents>  {
  
   typedef data_structures::CycleStartFromLastChange<Cycle> CycleWrap;
   typedef TwoLocalSearchAdapter<CycleWrap> CycleAdapt;
   typedef LocalSearchStepMultiSolution<CycleAdapt, SearchComponents> base;

   public:

        TwoLocalSearchStep(Cycle c, SearchComponents sc) 
           : base(CycleAdapt(m_cycleS), std::move(sc)), m_cycle(std::move(c)), m_cycleS(m_cycle) {}

   private:
       Cycle m_cycle;
       CycleWrap m_cycleS;
};

/**
 * @brief make template function for TwoLocalComponents, just to avoid providing type names in template.
 *
 *
 * @tparam Gain
 * @tparam GetNeighborhood
 * @param c
 * @param ich
 * @param ng
 *
 * @return 
 */
template <typename Gain, 
          typename GetNeighborhood = TrivialNeigborGetter,
          typename StopCondition = TrivialStopConditionMultiSolution>

TwoLocalComponents<Gain, GetNeighborhood, StopCondition>  

    make_TwoLocalSearchComponents(Gain ch, 
            GetNeighborhood ng = TrivialNeigborGetter(),
            StopCondition sc = TrivialStopConditionMultiSolution()) {

    return TwoLocalComponents<Gain, GetNeighborhood, StopCondition>(std::move(ch), std::move(ng), std::move(sc));
}


template <typename Metric>
decltype(make_TwoLocalSearchComponents(Gain2Opt<Metric>(std::declval<Metric>())))
getDefaultTwoLocalComponents(const Metric & m) {
    return make_TwoLocalSearchComponents(Gain2Opt<Metric>(m));
}


} //two_local_search
} //local_search
} //paal

#endif // TWO_LOCAL_SEARCH_HPP

