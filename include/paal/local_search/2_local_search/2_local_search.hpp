/**
 * @file 2_local_search.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */

#ifndef TWO_LOCAL_SEARCH_HPP
#define TWO_LOCAL_SEARCH_HPP

#include "paal/local_search/search_components.hpp"
#include "paal/local_search/local_search.hpp"
#include "paal/local_search/2_local_search/2_local_search_components.hpp"
#include "paal/local_search/2_local_search/2_local_search_solution_adapter.hpp"
#include "paal/data_structures/cycle/cycle_start_from_last_change.hpp"
#include "paal/data_structures/cycle/cycle_concept.hpp"

namespace paal {
namespace local_search {
namespace two_local_search {

/**
 @brief represents step of 2 local search in multi solution where Solution is Cycle, SolutionElement is pair of vertices and Move type is pair of vertices.
 See \ref local_search_page. There are three ways to provide search components
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

template <typename... Args>
using TwoLocalComponents = data_structures::Components<
            Gain,
            data_structures::NameWithDefault<GetMoves, TwoLocalSearchGetMoves>,
            data_structures::NameWithDefault<Commit, TwoLocalSearchCommit>
                >::type<Args...>;

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
          typename GetMoves = TwoLocalSearchGetMoves>
TwoLocalComponents<Gain, GetMoves>

    make_TwoLocalSearchComponents(Gain ch,
            GetMoves ng = GetMoves()) {

    return TwoLocalComponents<Gain, GetMoves>(std::move(ch), std::move(ng));
}


/**
 * @brief get default two local search components
 *
 * @tparam Metric is model of \ref metric concept
 * @param m metric
 */
template <typename Metric>
decltype(make_TwoLocalSearchComponents(GainTwoOpt<Metric>(std::declval<Metric>())))
getDefaultTwoLocalComponents(const Metric & m) {
    return make_TwoLocalSearchComponents(GainTwoOpt<Metric>(m));
}

/**
 * @brief local search for two - opt in tsp adapts tsp to local_search_multi_solution
 *
 * @tparam SearchStrategy
 * @tparam PostSearchAction
 * @tparam GlobalStopCondition
 * @tparam Cycle
 * @tparam Components
 * @param cycle
 * @param psa
 * @param gsc
 * @param components
 *
 * @return
 */
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
    return local_search(cycleAdapted, std::move(psa), std::move(gsc), std::move(components)...);
}

/**
 * @brief simple version of two_local_search
 *
 * @tparam SearchStrategy
 * @tparam Cycle
 * @tparam Components
 * @param cycle
 * @param components
 *
 * @return
 */
template <typename SearchStrategy = search_strategies::ChooseFirstBetter,
          typename Cycle,
          typename... Components>
bool two_local_search_simple(Cycle & cycle, Components... components) {
    return two_local_search(cycle, utils::SkipFunctor(), utils::ReturnFalseFunctor(), std::move(components)...);
}


} //two_local_search
} //local_search
} //paal

#endif // TWO_LOCAL_SEARCH_HPP

