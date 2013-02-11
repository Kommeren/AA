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

namespace detail {
    template <typename F, typename Solution, typename Update, typename SolutionUpdater> class Fun2Check {
            typedef decltype(std::declval<F>().value(std::declval<Solution>(),std::declval<Update>())) Dist;
        public:
            Fun2Check(F f, const SolutionUpdater & su) : std::move(f), m_solutionUpdaterFunctor(su) {}

            Dist gain(const Solution &s , const Update &u) {
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
          typename SearchStrategy = search_startegies::ChooseFirstBetter> 

class LocalSearchFunctionStep : 
    public LocalSearchStepBase<
                Solution, 
                NeighborhoodGetter, 
                detail::Fun2Check< 
                    ObjectiveFunction, 
                    Solution, 
                    typename local_search_concepts::NeighborhoodGetter<
                        NeighborhoodGetter, Solution>::Update, 
                    SolutionUpdater>, 
                SolutionUpdater, 
                StopCondition,
                SearchStrategy> {
                    

    typedef typename local_search_concepts::NeighborhoodGetter<
         NeighborhoodGetter, Solution>::Update Update;
    typedef detail::Fun2Check< 
                    ObjectiveFunction, 
                    Solution, 
                    Update, 
                    SolutionUpdater> UpdateChecker;
    typedef LocalSearchStepBase<
                Solution, 
                NeighborhoodGetter,
                UpdateChecker,
                SolutionUpdater, 
                StopCondition,
                SearchStrategy> base;
    public:
    LocalSearchFunctionStep(Solution s, NeighborhoodGetter n, ObjectiveFunction f, 
                            SolutionUpdater su, StopCondition sc, SearchStrategy ss) :  
        base(std::move(s), std::move(n), 
        UpdateChecker(std::move(f), base::m_solutionUpdaterFunctor), 
        std::move(su), std::move(sc), std:move(ss)) {} 

};

} //paal

#endif /* LOCAL_SEARCH_SINGLE_SOLUTION_OBJ_FUNCTION_HPP */
