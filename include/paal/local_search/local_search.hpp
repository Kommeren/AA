/**
 * @file local_search.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-03-04
 */
#ifndef LOCAL_SEARCH_HPP
#define LOCAL_SEARCH_HPP
#include "paal/utils/functors.hpp"
#include "single_solution/local_search_single_solution.hpp"
#include "single_solution/local_search_single_solution_obj_function.hpp"
#include "multi_solution/local_search_multi_solution.hpp"

namespace paal {
namespace local_search {

/**
 * @brief local search schema, common for
 * both single_solution and multi_solution versions
 *
 * @tparam LocalSearchStep
 * @tparam PostSearchAction
 * @tparam GlobalStopCondition
 * @param lss
 * @param psa
 * @param gsc
 *
 * @return true if the search was successful
 */
template <typename LocalSearchStep,
          typename PostSearchAction = utils::SkipFunctor,
          typename GlobalStopCondition = utils::ReturnFalseFunctor>
bool search(LocalSearchStep & lss,
            PostSearchAction psa = utils::SkipFunctor(),
            GlobalStopCondition gsc = utils::ReturnFalseFunctor()) {
    if(!lss.search()) {
        return false;
    }

    if(!gsc(lss.getSolution())) {
        psa(lss.getSolution());
        while(lss.search() && !gsc(lss.getSolution())) {
            psa(lss.getSolution());
        }
    }
    return true;
}




}//local_search
}//paal
#endif /* LOCAL_SEARCH_HPP */
