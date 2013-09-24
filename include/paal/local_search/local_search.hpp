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

template <typename LocalSearchStep, 
          typename PostSearchAction = utils::DoNothingFunctor,
          typename GlobalStopCondition = utils::ReturnFalseFunctor>
bool search(LocalSearchStep & lss, 
            PostSearchAction psa = utils::DoNothingFunctor(),
            GlobalStopCondition gsc = utils::ReturnFalseFunctor()) {
    bool ret = false;  
    while(lss.search() && !gsc(lss.getSolution())) {
        ret = true;
        psa(lss.getSolution());
    }
    return ret;
}




}//local_search
}//paal
#endif /* LOCAL_SEARCH_HPP */
