/*
 *  * local_search.hpp
 *  *
 *  *  Created on: 03-01-2013
 *  *      Author: Piotr Wygocki
 *  */

#ifndef __LOCAL_SEARCH_SINGLE_SOLUTION__
#define __LOCAL_SEARCH_SINGLE_SOLUTION__

#include <utility>
#include <algorithm>
#include <functional>

#include "local_search_concepts.hpp"

namespace paal {
namespace local_search {

namespace search_startegies {
    class ChooseFirstBetter;
    class SteepestSlope;
}


template <typename Solution, 
          typename NeighbourGetter, 
          typename CheckIfImprove, 
          typename SolutionUpdater, 
          typename SearchStrategy = search_startegies::ChooseFirstBetter> 

class LocalSearchStep {
    BOOST_CONCEPT_ASSERT((local_search_concepts::NeighbourGetter<NeighbourGetter, Solution>));
    BOOST_CONCEPT_ASSERT((local_search_concepts::CheckIfImprove<CheckIfImprove, Solution, NeighbourGetter>));
    BOOST_CONCEPT_ASSERT((local_search_concepts::SolutionUpdater<SolutionUpdater, Solution, NeighbourGetter>));
    static_assert(std::is_same<SearchStrategy, search_startegies::ChooseFirstBetter>::value || 
                  std::is_same<SearchStrategy, search_startegies::SteepestSlope>::value, "Wrong search strategy");
    
    typedef typename local_search_concepts::
        NeighbourGetter<NeighbourGetter, Solution>::UpdateElement UpdateElement;
public:
        LocalSearchStep(Solution solution, NeighbourGetter ng, 
                                     CheckIfImprove check, SolutionUpdater solutionUpdater) :
            m_solution(std::move(solution)), m_neighbourGetterFunctor(std::move(ng)), 
            m_checkFunctor(std::move(check)), m_solutionUpdaterFunctor(std::move(solutionUpdater)), m_lastSearchSucceded(false) {}

        bool search() {
            auto adjustmentSet = m_neighbourGetterFunctor.getNeighbourhood(m_solution);

            std::find_if(adjustmentSet.first, adjustmentSet.second, [&](const UpdateElement & update) {
                if(m_checkFunctor.checkIfImproved(m_solution, update) > 0) {
                    m_lastSearchSucceded = true;
                    m_solutionUpdaterFunctor.update(m_solution, update);
                }   
                return m_lastSearchSucceded;
            });
            return m_lastSearchSucceded;
        }
   
private:
    Solution m_solution;
    NeighbourGetter m_neighbourGetterFunctor;
    CheckIfImprove m_checkFunctor;
    SolutionUpdater m_solutionUpdaterFunctor;
    bool m_lastSearchSucceded;
};

template <typename Solution, typename NeighbourGetter, typename CheckIfImprove, typename SolutionUpdater> 
class LocalSearchStep<Solution, NeighbourGetter, CheckIfImprove, SolutionUpdater, search_startegies::SteepestSlope> {
    //TODO implement
};

} // local_search
} // paal

#endif // __LOCAL_SEARCH_SINGLE_SOLUTION__
