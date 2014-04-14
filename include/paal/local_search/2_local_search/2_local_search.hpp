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
       <li> use (TODO link does not generate) get_default_two_local_components(const Metric &) - this is the easiest way.
       <li> use TwoLocalcomponents to provide your own search components
       <li> write your own implementation of Multisearch_components
 </ul>
 Basic usage of this algorithm is extremely simple and elegant. <br> We are using some helper functions from the library.

 \snippet 2_local_search_example.cpp Two Local Search Example

 Although the basic usage is very simple, the sophisticated user can still easily change default parameters and exchange them with his ones. <br>
*
* @tparam Cycle input cycle, hast to be model of the  \ref cycle concept
* @tparam search_components this is model Multisearch_components
*/

template <typename... Args>
using TwoLocalcomponents = data_structures::components<
            Gain,
            data_structures::NameWithDefault<GetMoves, two_local_searchget_moves>,
            data_structures::NameWithDefault<Commit, two_local_search_commit>
                >::type<Args...>;

/**
 * @brief make template function for TwoLocalcomponents, just to avoid providing type names in template.
 *
 * @tparam Gain
 * @tparam get_moves
 * @param c
 * @param ich
 * @param ng
 *
 * @return
 */
template <typename Gain,
          typename GetMoves = two_local_searchget_moves>
TwoLocalcomponents<Gain, GetMoves>

    make_two_local_search_components(Gain ch,
            GetMoves gm = GetMoves{}) {

    return TwoLocalcomponents<Gain, GetMoves>(std::move(ch), std::move(gm));
}


/**
 * @brief get default two local search components
 *
 * @tparam Metric is model of \ref metric concept
 * @param m metric
 */
template <typename Metric>
decltype(make_two_local_search_components(gain_two_opt<Metric>(std::declval<Metric>())))
get_default_two_local_components(const Metric & m) {
    return make_two_local_search_components(gain_two_opt<Metric>(m));
}

/**
 * @brief local search for two - opt in tsp adapts tsp to local_search_multi_solution
 *
 * @tparam SearchStrategy
 * @tparam PostSearchAction
 * @tparam GlobalStopCondition
 * @tparam Cycle
 * @tparam components
 * @param cycle
 * @param psa
 * @param gsc
 * @param components
 *
 * @return
 */
template <typename SearchStrategy,
          typename PostSearchAction,
          typename GlobalStopCondition,
          typename Cycle,
          typename... components>
bool two_local_search(
            Cycle & cycle,
            SearchStrategy searchStrategy,
            PostSearchAction psa,
            GlobalStopCondition gsc,
            components... comps) {
    typedef data_structures::cycle_start_from_last_change<Cycle> CSFLCh;
    CSFLCh cycleSFLCh(cycle);
    local_search::two_local_search::two_local_search_adapter<CSFLCh> cycleAdapted(cycleSFLCh);
    return local_search(cycleAdapted, std::move(searchStrategy), std::move(psa), std::move(gsc), std::move(comps)...);
}

/**
 * @brief simple version of two_local_search
 *
 * @tparam SearchStrategy
 * @tparam Cycle
 * @tparam components
 * @param cycle
 * @param components
 *
 * @return
 */
template <typename Cycle,
          typename... components>
bool two_local_search_simple(Cycle & cycle, components... comps) {
    return two_local_search(cycle, choose_first_better_strategy{},
                utils::skip_functor{},
                utils::return_false_functor{},
                std::move(comps)...);
}


} //two_local_search
} //local_search
} //paal

#endif // TWO_LOCAL_SEARCH_HPP

