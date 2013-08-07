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
#include "paal/local_search/search_components.hpp"
#include "paal/local_search/multi_solution/local_search_multi_solution.hpp"
#include "paal/local_search/2_local_search/2_local_search_commit.hpp"
#include "paal/local_search/2_local_search/2_local_search_checker.hpp"
#include "paal/local_search/2_local_search/2_local_search_solution_adapter.hpp"
#include "paal/data_structures/cycle/cycle_start_from_last_change.hpp"
#include "paal/data_structures/cycle/cycle_concept.hpp"

namespace paal {
namespace local_search {
namespace two_local_search {

/**
 @brief represents step of 2 local search in multi solution where Solution is Cycle, SolutionElement is pair of vertices and Move type is pair of vertices.
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



typedef data_structures::Components<
            Gain, 
            data_structures::NameWithDefault<GetMoves, TrivialNeighborGetter>,
            data_structures::NameWithDefault<StopCondition, utils::ReturnFalseFunctor>,
            data_structures::NameWithDefault<Commit, TwoLocalSearchCommit>
                > TwoLocalSearchComponentsCreator;

template <typename Gain, 
          typename GetMoves = TrivialNeighborGetter, 
          typename StopCondition = utils::ReturnFalseFunctor> 
using TwoLocalComponents = typename TwoLocalSearchComponentsCreator::type<Gain, GetMoves, StopCondition>;
/*    public MultiSearchComponents<
            GetMoves, 
            Gain, 
            TwoLocalSearchCommit, 
            StopCondition> {

    typedef MultiSearchComponents<
            GetMoves, 
            Gain, 
            TwoLocalSearchCommit, 
            StopCondition> base; 
public : 
    TwoLocalComponents(Gain ic = Gain(), 
                       GetMoves ng = GetMoves(), 
                       StopCondition sc = StopCondition()) : 
            base(std::move(ng), std::move(ic), TwoLocalSearchCommit(), std::move(sc)) {}

};*/

/**
 * @brief make template function for TwoLocalComponents, just to avoid providing type names in template.
 *
 * @tparam Gain
 * @tparam GetMoves
 * @param c
 * @param ich
 * @param ng
 *
 * @return 
 */
template <typename Gain, 
          typename GetMoves = TrivialNeighborGetter,
          typename StopCondition = utils::ReturnFalseFunctor>
TwoLocalComponents<Gain, GetMoves, StopCondition>  

    make_TwoLocalSearchComponents(Gain ch, 
            GetMoves ng = TrivialNeighborGetter(),
            StopCondition sc = utils::ReturnFalseFunctor()) {

    return TwoLocalComponents<Gain, GetMoves, StopCondition>(std::move(ch), std::move(ng), std::move(sc));
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

template <typename SearchStrategy = search_strategies::ChooseFirstBetter,
          typename PostSearchAction,
          typename GlobalStopCondition,
          typename Cycle,
          typename... Components>
bool two_local_search(
            Cycle & cycle,
            PostSearchAction psa,
            GlobalStopCondition gsc,
            Components... components) {
    typedef data_structures::CycleStartFromLastChange<Cycle> CSFLCh;
    CSFLCh cycleSFLCh(cycle);
    local_search::two_local_search::TwoLocalSearchAdapter<CSFLCh> cycleAdapted(cycleSFLCh);
    return local_search_multi_solution(cycleAdapted, std::move(psa), std::move(gsc), std::move(components)...);
}

template <typename SearchStrategy = search_strategies::ChooseFirstBetter, 
          typename Cycle, 
          typename... Components>
bool two_local_search_simple(Cycle & cycle, Components... components) {
    return two_local_search(cycle, utils::DoNothingFunctor(), utils::ReturnFalseFunctor(), std::move(components)...);
}


} //two_local_search

/*template <typename Gain, typename GetMoves, typename StopCondition> 
struct SearchComponentsTraits<two_local_search::TwoLocalComponents<Gain, GetMoves, StopCondition>> 
        : SearchComponentsTraits<
            MultiSearchComponents<GetMoves, Gain, two_local_search::TwoLocalSearchCommit, StopCondition>
                               > {};*/



} //local_search
} //paal

#endif // TWO_LOCAL_SEARCH_HPP

