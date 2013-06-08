/**
 * @file 2_local_search.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */

#ifndef TWO_LOCAL_SEARCH_HPP
#define TWO_LOCAL_SEARCH_HPP

#include "paal/local_search/multi_solution_step/trivial_neighbor.hpp"
#include "paal/local_search/search_components.hpp"
#include "paal/local_search/multi_solution_step/local_search_multi_solution.hpp"
#include "paal/local_search/2_local_search/2_local_search_updater.hpp"
#include "paal/local_search/2_local_search/2_local_search_checker.hpp"
#include "paal/local_search/2_local_search/2_local_search_solution_adapter.hpp"
#include "paal/data_structures/cycle/cycle_start_from_last_change.hpp"
#include "paal/data_structures/cycle/cycle_concept.hpp"

namespace paal {
namespace local_search {
namespace two_local_search {

/**
 @brief represents step of 2 local search in multi solution where Solution is Cycle, SolutionElement is pair of vertices and Update type is pair of vertices.
 See \ref local_search. There are three ways to provide search components
 <ul>
       <li> use (TODO link does not generate) getDefaultTwoLocalComponents(const Metric &) - this is the easiest way.  
       <li> use TwoLocalComponents to provide your own search Components
       <li> write your own implementation of MultiSearchComponents
 </ul>
 Basic usage of this algorithm is extremely simple and elegant. <br> We are using some helper functions from the library.

 \snippet 2_local_search_example.cpp Two Local Search Example

 Although the basic usage is very simple, the sophisticated user can still easily change default parameters and exchange them with his ones. <br>
*
* @tparam Cycle input cycle, hast to be model of the  \ref cycle concept
* @tparam SearchComponents this is model MultiSearchComponents
*/
template <typename Cycle,
          typename SearchComponents>
class  TwoLocalSearchStep : 
   public LocalSearchStepMultiSolution<
            TwoLocalSearchAdapter<
                data_structures::CycleStartFromLastChange<Cycle>>, 
                search_strategies::ChooseFirstBetter,
                SearchComponents>  {

    BOOST_CONCEPT_ASSERT((data_structures::concepts::Cycle<Cycle>));
  
    typedef data_structures::CycleStartFromLastChange<Cycle> CycleWrap;
    typedef TwoLocalSearchAdapter<CycleWrap> CycleAdapt;
    typedef LocalSearchStepMultiSolution<CycleAdapt, search_strategies::ChooseFirstBetter, SearchComponents> base;

    public:

        TwoLocalSearchStep(Cycle c, SearchComponents sc) 
           : base(CycleAdapt(m_cycleS), std::move(sc)), m_cycle(std::move(c)), m_cycleS(m_cycle) {}

    private:
       Cycle m_cycle;
       CycleWrap m_cycleS;
};

template <typename Gain, 
          typename GetNeighborhood = TrivialNeighborGetter, 
          typename StopCondition = utils::ReturnFalseFunctor> 
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

/**
 * @brief make template function for TwoLocalComponents, just to avoid providing type names in template.
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
          typename GetNeighborhood = TrivialNeighborGetter,
          typename StopCondition = utils::ReturnFalseFunctor>
TwoLocalComponents<Gain, GetNeighborhood, StopCondition>  

    make_TwoLocalSearchComponents(Gain ch, 
            GetNeighborhood ng = TrivialNeighborGetter(),
            StopCondition sc = utils::ReturnFalseFunctor()) {

    return TwoLocalComponents<Gain, GetNeighborhood, StopCondition>(std::move(ch), std::move(ng), std::move(sc));
}


/**
 * @brief get default two local search components
 *
 * @tparam Metric is model of \ref metric concept
 * @param m metric
 */
template <typename Metric>
decltype(make_TwoLocalSearchComponents(Gain2Opt<Metric>(std::declval<Metric>())))
getDefaultTwoLocalComponents(const Metric & m) {
    return make_TwoLocalSearchComponents(Gain2Opt<Metric>(m));
}


} //two_local_search
} //local_search
} //paal

#endif // TWO_LOCAL_SEARCH_HPP

