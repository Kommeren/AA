/**
 * @file local_search_multi_solution.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */
#ifndef __LOCAL_SEARCH_MULTI_SOLUTION__
#define __LOCAL_SEARCH_MULTI_SOLUTION__

#include <utility>
#include <algorithm>
#include <functional>

#include "local_search_multi_solution_concepts.hpp"
#include "trivial_stop_condition_multi_solution.hpp"

namespace paal {
namespace local_search {

namespace search_startegies {
    class ChooseFirstBetter;
    class SteepestSlope;
}


/**
 * @class LocalSearchStepMultiSolution
 * @brief General class for local search on the multi solution. Note there is no Update type here because it can be deduced.
 *
 * @tparam Solution
 * @tparam NeighborhoodGetter
 * @tparam ImproveChecker
 * @tparam SolutionUpdater
 * @tparam StopCondition
 * @tparam SearchStrategy
           Search strategy descibes LS search strategy. For ow we are planning two strategies: 
           <ul>
           <li>ChooseFirstBetter -> The algorithm chooses the first update with the positive gain
           <li>SteepestSlope     -> The algorithm chooses the update with the largest gain and update if positive.
           </ul>
 */

template <typename Solution, 
          typename NeighborhoodGetter, 
          typename ImproveChecker, 
          typename SolutionUpdater, 
          typename StopCondition = TrivialStopConditionMultiSolution,
          typename SearchStrategy = search_startegies::ChooseFirstBetter
          >
class LocalSearchStepMultiSolution {
      BOOST_CONCEPT_ASSERT((local_search_concepts::MultiSolution<Solution>));
      BOOST_CONCEPT_ASSERT((local_search_concepts::MultiNeighborhoodGetter<NeighborhoodGetter, Solution>));
      BOOST_CONCEPT_ASSERT((local_search_concepts::MultiImproveChecker<ImproveChecker, Solution, NeighborhoodGetter>));
      BOOST_CONCEPT_ASSERT((local_search_concepts::MultiSolutionUpdater<SolutionUpdater, Solution, NeighborhoodGetter>));
      static_assert(std::is_same<SearchStrategy, search_startegies::ChooseFirstBetter>::value || 
                    std::is_same<SearchStrategy, search_startegies::SteepestSlope>::value, "Wrong search strategy");
    
    typedef typename local_search_concepts::
        MultiSolution<Solution>::Element SolutionElement;
    typedef typename local_search_concepts::
        MultiNeighborhoodGetter<NeighborhoodGetter, Solution>::Update Update;
    
    typedef LocalSearchStepMultiSolution<Solution, NeighborhoodGetter, ImproveChecker, SolutionUpdater, StopCondition, SearchStrategy>  self;
    
public:

    /**
     * @brief costam 
     *
     * @param solution
     * @param ng
     * @param check
     * @param solutionUpdater
     * @param sc
     */
    LocalSearchStepMultiSolution(Solution solution, NeighborhoodGetter ng, 
                                 ImproveChecker check, SolutionUpdater solutionUpdater,
                                 StopCondition sc = TrivialStopConditionMultiSolution()) :
     m_solution(std::move(solution)), m_neighborGetterFunctor(std::move(ng)), 
     m_checkFunctor(std::move(check)), m_solutionUpdaterFunctor(std::move(solutionUpdater)), 
     m_stopConditionFunctor(std::move(sc)),  m_lastSearchSucceded(false), m_stop(false) {}

    /**
     * @brief tata 
     *
     * @return 
     */
    bool search() {
        m_lastSearchSucceded = false;

        auto check = std::bind(std::mem_fun(&self::checkForUpdate), this, std::placeholders::_1);
        std::find_if(m_solution.begin(), m_solution.end(), check);
        
        return m_lastSearchSucceded;
    }

    // TODO it is not optional :)
    typename std::enable_if<has_get<Solution>::value, decltype(std::declval<Solution>().get()) &>::type getSolution() {
        return m_solution.get();
    }
    
    
    //TODO doesn't work
/*    std::enable_if<!has_get<Solution>::value, Solution &> getSolution() {
        return m_solution;
    }*/

private:
    bool checkForUpdate(const SolutionElement & r) {
         
        auto adjustmentSet = m_neighborGetterFunctor.get(m_solution, r);

        std::find_if(adjustmentSet.first, adjustmentSet.second, [&](const Update & update) {
            if(m_checkFunctor.gain(m_solution, r, update) > 0) {
                m_lastSearchSucceded = true;
                m_solutionUpdaterFunctor.update(m_solution, r, update);
            } else {
                if(m_stopConditionFunctor.stop(m_solution, r, update)) {
                m_stop = true;
                }
            }
            return m_lastSearchSucceded && !m_stop;
        });
          
        return m_lastSearchSucceded && !m_stop;
    }

    Solution m_solution;
    NeighborhoodGetter m_neighborGetterFunctor;
    ImproveChecker m_checkFunctor;
    SolutionUpdater m_solutionUpdaterFunctor;
    StopCondition m_stopConditionFunctor;
    bool m_lastSearchSucceded;
    bool m_stop;
};

template <typename Solution, typename NeighborhoodGetter, typename ImproveChecker, typename SolutionUpdater, typename StopCondition> 
class LocalSearchStepMultiSolution<Solution, NeighborhoodGetter, ImproveChecker, SolutionUpdater, StopCondition, search_startegies::SteepestSlope> {
    //TODO implement
};

} // local_search
} // paal

#endif // __LOCAL_SEARCH_MULTI_SOLUTION__
