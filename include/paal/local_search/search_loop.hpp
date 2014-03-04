/**
 * @file search_loop.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2014-03-04
 */
#ifndef SEARCH_LOOP_HPP
#define SEARCH_LOOP_HPP

#include "paal/utils/functors.hpp"

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
#endif /* SEARCH_LOOP_HPP */
