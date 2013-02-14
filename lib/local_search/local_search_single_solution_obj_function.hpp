/**
 * @file local_search_single_solution_obj_function.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-11
 */
#ifndef LOCAL_SEARCH_SINGLE_SOLUTION_OBJ_FUNCTION_HPP
#define LOCAL_SEARCH_SINGLE_SOLUTION_OBJ_FUNCTION_HPP

#include "local_search_single_solution.hpp"
#include "trivial_solution_updater.hpp"

namespace paal {
namespace local_search {

namespace detail {
    template <typename F, typename Solution, typename SolutionUpdater> class Fun2Check {
            typedef decltype(std::declval<F>()(std::declval<Solution>())) Dist;
        public:
            Fun2Check(F f, const SolutionUpdater & su) : m_f(std::move(f)), m_solutionUpdaterFunctor(su) {}

            template <typename Update> Dist gain(const Solution &s , const Update &u) {
                Solution newS(s);
                m_solutionUpdaterFunctor.update(newS, u);
                return m_f(newS) - m_f(s);
            }


        private:

            F m_f;
            const SolutionUpdater & m_solutionUpdaterFunctor;
    };
}

template <typename Solution, 
          typename NeighborhoodGetter, 
          typename ObjectiveFunction, 
          typename SolutionUpdater = TrivialSolutionUpdater,
          typename StopCondition = TrivialStopCondition,
          typename SearchStrategy = search_strategies::ChooseFirstBetter> 

class LocalSearchFunctionStep : 
    public LocalSearchStep<
                Solution, 
                NeighborhoodGetter, 
                detail::Fun2Check< 
                    ObjectiveFunction, 
                    Solution, 
                    SolutionUpdater>, 
                SolutionUpdater, 
                StopCondition,
                SearchStrategy> {
                    
    typedef detail::Fun2Check< 
                    ObjectiveFunction, 
                    Solution, 
                    SolutionUpdater> UpdateChecker;
    typedef LocalSearchStep<
                Solution, 
                NeighborhoodGetter,
                UpdateChecker,
                SolutionUpdater, 
                StopCondition,
                SearchStrategy> base;
    public:
    LocalSearchFunctionStep(Solution s = Solution(), NeighborhoodGetter n = NeighborhoodGetter(), ObjectiveFunction f = ObjectiveFunction(), 
                            SolutionUpdater su = SolutionUpdater(), StopCondition sc = StopCondition()) :  
        base(std::move(s), std::move(n), 
        UpdateChecker(std::move(f), base::m_solutionUpdaterFunctor), 
        std::move(su), std::move(sc)) {} 

};

} //local_search
} //paal

#endif /* LOCAL_SEARCH_SINGLE_SOLUTION_OBJ_FUNCTION_HPP */
