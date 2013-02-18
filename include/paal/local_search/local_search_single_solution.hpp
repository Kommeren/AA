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
#include "trivial_stop_condition.hpp"
#include "paal/helpers/iterator_helpers.hpp"

namespace paal {
namespace local_search {

namespace search_strategies {
    class ChooseFirstBetter;
    class SteepestSlope;
}



template <typename Solution, 
          typename NeighborhoodGetter, 
          typename ImproveChecker, 
          typename SolutionUpdater,
          typename StopCondition> 

class LocalSearchStepBase {
    BOOST_CONCEPT_ASSERT((local_search_concepts::NeighborhoodGetter<NeighborhoodGetter, Solution>));
    BOOST_CONCEPT_ASSERT((local_search_concepts::ImproveChecker<ImproveChecker, Solution, NeighborhoodGetter>));
    BOOST_CONCEPT_ASSERT((local_search_concepts::SolutionUpdater<SolutionUpdater, Solution, NeighborhoodGetter>));

protected:
    LocalSearchStepBase(Solution solution, NeighborhoodGetter ng, 
                        ImproveChecker check, SolutionUpdater solutionUpdater, 
                        StopCondition sc = StopCondition()) :
        m_solution(std::move(solution)), m_neighborGetterFunctor(std::move(ng)), 
        m_checkFunctor(std::move(check)), m_solutionUpdaterFunctor(std::move(solutionUpdater)),
        m_stopConditionFunctor(std::move(sc)) {}

   
    Solution m_solution;
    NeighborhoodGetter m_neighborGetterFunctor;
    ImproveChecker m_checkFunctor;
    SolutionUpdater m_solutionUpdaterFunctor;
    StopCondition m_stopConditionFunctor;
    bool m_lastSearchSucceded;

public:
    typedef typename local_search_concepts::
        NeighborhoodGetter<NeighborhoodGetter, Solution>::Update Update;
    
    Solution & getSolution() {
        return m_solution;
    }
};

template <typename Solution, 
          typename NeighborhoodGetter, 
          typename ImproveChecker, 
          typename SolutionUpdater,
          typename StopCondition = TrivialStopCondition,
          typename SearchStrategy = search_strategies::ChooseFirstBetter> 

class LocalSearchStep : public LocalSearchStepBase<Solution, NeighborhoodGetter, 
                                                   ImproveChecker, SolutionUpdater, 
                                                   StopCondition> {
    
        static_assert(std::is_same<SearchStrategy, search_strategies::ChooseFirstBetter>::value || 
                      std::is_same<SearchStrategy, search_strategies::SteepestSlope>::value, "Wrong search strategy");
        typedef LocalSearchStepBase<Solution, NeighborhoodGetter, 
                                    ImproveChecker, SolutionUpdater, 
                                    StopCondition> base;
public:
        LocalSearchStep(Solution solution = Solution(), NeighborhoodGetter ng = NeighborhoodGetter(), 
                        ImproveChecker check = ImproveChecker(), SolutionUpdater solutionUpdater = SolutionUpdater(), 
                        StopCondition sc = StopCondition()) :
            base(std::move(solution), std::move(ng), 
                 std::move(check), std::move(solutionUpdater),
                 std::move(sc)) {}
        typedef typename base::Update Update;

        bool search() {
            auto adjustmentSet = this->m_neighborGetterFunctor.get(this->m_solution);

            for(const Update & update : helpers::make_range(adjustmentSet)) {
                if(this->m_checkFunctor.gain(this->m_solution, update) > 0) {
                    this->m_solutionUpdaterFunctor.update(this->m_solution, update);
                    return true;
                } else {
                    if(this->m_stopConditionFunctor.stop(this->m_solution, update)) {
                        break;
                    }
                }
            }
            return false;
        }
};

template <typename Solution, typename NeighborhoodGetter, 
          typename ImproveChecker, typename SolutionUpdater, typename StopCondition> 
class LocalSearchStep<Solution, NeighborhoodGetter, ImproveChecker, SolutionUpdater, StopCondition, search_strategies::SteepestSlope> 
        : public LocalSearchStepBase<Solution, NeighborhoodGetter, 
                                ImproveChecker, SolutionUpdater, 
                                StopCondition>  {
    typedef LocalSearchStepBase<Solution, NeighborhoodGetter, 
                                ImproveChecker, SolutionUpdater, 
                                StopCondition> base;
public:
    LocalSearchStep(Solution solution = Solution(), NeighborhoodGetter ng = NeighborhoodGetter(), 
                    ImproveChecker check = ImproveChecker(), SolutionUpdater solutionUpdater = SolutionUpdater(), 
                    StopCondition sc = StopCondition()) :
        base(std::move(solution), std::move(ng), 
             std::move(check), std::move(solutionUpdater),
             std::move(sc)) {}

    bool search() {
        int max = INT_MIN;
        auto adjustmentSet = this->m_neighborGetterFunctor.get(this->m_solution);
        auto b = adjustmentSet.first;
        auto best = b;

        for(;b !=  adjustmentSet.second; ++b) {
            int t = this->m_checkFunctor.gain(this->m_solution, *b);
            if(t > max) {
                best = b;
                max = t;
            } else {
                if(this->m_stopConditionFunctor.stop(this->m_solution, *b)) {
                    break;
                }

            }
        }
        if(max > 0) {
            this->m_solutionUpdaterFunctor.update(this->m_solution, *best);
        }
        return max > 0;
    }
};

} // local_search
} // paal

#endif // __LOCAL_SEARCH_SINGLE_SOLUTION__
