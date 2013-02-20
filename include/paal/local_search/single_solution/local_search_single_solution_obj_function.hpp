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
#include "search_obj_func_components.hpp"

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

    template <typename SearchObjFunctionComponents, typename Solution>
    class SearchObjFunctionComponentsToSearchComponents {
    private:
        typedef SearchObjFunctionComponentsTraits<
                    SearchObjFunctionComponents> traits; 
    public:
        typedef detail::Fun2Check< 
                        typename traits::ObjectiveFunction, 
                        Solution, 
                        typename traits::SolutionUpdater> ImproveCheckerType;
        typedef SearchComponents<
                    typename traits::NeighborhoodGetter, 
                             ImproveCheckerType,
                    typename traits::SolutionUpdater, 
                    typename traits::StopCondition>  type;
    };
}


//TODO concepts !!!
template <typename Solution,
          typename SearchObjFunctionComponents,
          typename SearchStrategy = search_strategies::ChooseFirstBetter> 

class LocalSearchFunctionStep : 
    public LocalSearchStep<
                Solution,
                typename detail::SearchObjFunctionComponentsToSearchComponents<
                    SearchObjFunctionComponents,
                    Solution>::type,
                SearchStrategy
                > {
    typedef detail::SearchObjFunctionComponentsToSearchComponents<
        SearchObjFunctionComponents, Solution> Convert;
    typedef typename Convert::type SearchComponents;
    typedef typename Convert::ImproveCheckerType ImproveChecker;
    typedef LocalSearchStep<
                Solution, 
                SearchComponents,
                SearchStrategy> base;
    public:
    LocalSearchFunctionStep(Solution sol = Solution(), 
            SearchObjFunctionComponents s = SearchObjFunctionComponents()) :  
        base(std::move(sol), 
             SearchComponents
                   (
                    std::move(s.getNeighborhoodGetter()),
                    ImproveChecker(std::move(s.getObjectiveFunction()), base::m_searchComponents.getSolutionUpdater()),
                    std::move(s.getSolutionUpdater()),
                    std::move(s.getStopCondition())
                   )
            ) {} 
};

} //local_search
} //paal

#endif /* LOCAL_SEARCH_SINGLE_SOLUTION_OBJ_FUNCTION_HPP */
