/**
 * @file local_search_single_solution.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */

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
          typename NeighbourhoodGetter, 
          typename ImproveChecker, 
          typename SolutionUpdater, 
          typename SearchStrategy = search_startegies::ChooseFirstBetter> 

class LocalSearchStep {
    BOOST_CONCEPT_ASSERT((local_search_concepts::NeighbourhoodGetter<NeighbourhoodGetter, Solution>));
    BOOST_CONCEPT_ASSERT((local_search_concepts::ImproveChecker<ImproveChecker, Solution, NeighbourhoodGetter>));
    BOOST_CONCEPT_ASSERT((local_search_concepts::SolutionUpdater<SolutionUpdater, Solution, NeighbourhoodGetter>));
    static_assert(std::is_same<SearchStrategy, search_startegies::ChooseFirstBetter>::value || 
                  std::is_same<SearchStrategy, search_startegies::SteepestSlope>::value, "Wrong search strategy");
    
    typedef typename local_search_concepts::
        NeighbourhoodGetter<NeighbourhoodGetter, Solution>::Update Update;
public:
        LocalSearchStep(Solution solution, NeighbourhoodGetter ng, 
                                     ImproveChecker check, SolutionUpdater solutionUpdater) :
            m_solution(std::move(solution)), m_neighbourGetterFunctor(std::move(ng)), 
            m_checkFunctor(std::move(check)), m_solutionUpdaterFunctor(std::move(solutionUpdater)), m_lastSearchSucceded(false) {}

        bool search() {
            auto adjustmentSet = m_neighbourGetterFunctor.get(m_solution);

            std::find_if(adjustmentSet.first, adjustmentSet.second, [&](const Update & update) {
                if(m_checkFunctor.gain(m_solution, update) > 0) {
                    m_lastSearchSucceded = true;
                    m_solutionUpdaterFunctor.update(m_solution, update);
                }   
                return m_lastSearchSucceded;
            });
            return m_lastSearchSucceded;
        }
   
private:
    Solution m_solution;
    NeighbourhoodGetter m_neighbourGetterFunctor;
    ImproveChecker m_checkFunctor;
    SolutionUpdater m_solutionUpdaterFunctor;
    bool m_lastSearchSucceded;
};

template <typename Solution, typename NeighbourhoodGetter, typename ImproveChecker, typename SolutionUpdater> 
class LocalSearchStep<Solution, NeighbourhoodGetter, ImproveChecker, SolutionUpdater, search_startegies::SteepestSlope> {
    //TODO implement
};

} // local_search
} // paal

#endif // __LOCAL_SEARCH_SINGLE_SOLUTION__
