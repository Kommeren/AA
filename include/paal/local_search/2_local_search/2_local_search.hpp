/**
 * @file 2_local_search.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */

//TODO change all defines
#ifndef __2_LOCAL_SEARCH__
#define __2_LOCAL_SEARCH__

#include "paal/local_search/multi_solution/trivial_neighbor.hpp"
#include "paal/local_search/multi_solution/trivial_stop_condition_multi_solution.hpp"
#include "paal/local_search/search_components.hpp"
#include "paal/local_search/multi_solution/local_search_multi_solution.hpp"
#include "paal/local_search/2_local_search/2_local_search_updater.hpp"
#include "paal/local_search/2_local_search/2_local_search_checker.hpp"
#include "paal/local_search/2_local_search/2_local_search_solution_adapter.hpp"

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
    public LocalSearchStepMultiSolution<TwoLocalSearchContainer<Cycle>, SearchComponents>  {
  
   typedef LocalSearchStepMultiSolution<TwoLocalSearchContainer<Cycle> , SearchComponents> base;

   public:

        TwoLocalSearchStep(Cycle c, SearchComponents sc) 
           : base(TwoLocalSearchContainer<Cycle>(m_cycle), std::move(sc)), m_cycle(std::move(c)) {}

   private:
       Cycle m_cycle;
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

    make_TwoLocalSearchCOmponents(Gain ch, 
            GetNeighborhood ng = TrivialNeigborGetter(),
            StopCondition sc = TrivialStopConditionMultiSolution()) {

    return TwoLocalComponents<Gain, GetNeighborhood, StopCondition>(std::move(ch), std::move(ng), std::move(sc));
}


template <typename Metric>
decltype(make_TwoLocalSearchCOmponents(Gain2Opt<Metric>(std::declval<Metric>())))
getDefaultTwoLocalComponents(const Metric & m) {
    return make_TwoLocalSearchCOmponents(Gain2Opt<Metric>(m));
}


} //two_local_search
} //local_search
} //paal

#endif // __2_LOCAL_SEARCH__

