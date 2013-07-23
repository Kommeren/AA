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

template <typename SearchComponentsObjFun> 
struct SearchObjFunctionComponentsTraits {
    typedef typename data_structures::ComponentTraits<SearchComponentsObjFun>::template type<GetNeighborhood>::type GetNeighborhoodT; 
    typedef typename data_structures::ComponentTraits<SearchComponentsObjFun>::template type<ObjFunction>::type ObjFunctionT; 
    typedef typename data_structures::ComponentTraits<SearchComponentsObjFun>::template type<UpdateSolution>::type UpdateSolutionT; 
    typedef typename data_structures::ComponentTraits<SearchComponentsObjFun>::template type<StopCondition>::type StopConditionT; 
};

namespace detail {
    template <typename F, typename Solution, typename UpdateSolution> class Fun2Check {
            typedef decltype(std::declval<F>()(std::declval<Solution>())) Dist;
        public:
            Fun2Check(F f, const UpdateSolution & su) : m_f(std::move(f)), m_solutionUpdaterFunctor(su) {}

            template <typename Update> Dist operator()(const Solution &s , const Update &u) {
                Solution newS(s);
                m_solutionUpdaterFunctor(newS, u);
                return m_f(newS) - m_f(s);
            }

        private:

            F m_f;
            const UpdateSolution & m_solutionUpdaterFunctor;
    };

    template <typename SearchObjFunctionComponents, typename Solution>
    class SearchObjFunctionComponentsToSearchComponents {
    private:
        typedef SearchObjFunctionComponentsTraits<
                    SearchObjFunctionComponents> traits; 
    public:
        typedef detail::Fun2Check< 
                        typename traits::ObjFunctionT, 
                        Solution, 
                        typename traits::UpdateSolutionT> GainType;
        typedef SearchComponents<
                    typename traits::GetNeighborhoodT, 
                             GainType,
                    typename traits::UpdateSolutionT, 
                    typename traits::StopConditionT>  type;
    };
}


//TODO concepts !!!
template <typename Solution,
          typename SearchStrategy, 
          typename SearchObjFunctionComponents>

class LocalSearchFunctionStep : 
    public LocalSearchStep<
                Solution,
                SearchStrategy,
                typename detail::SearchObjFunctionComponentsToSearchComponents<
                    SearchObjFunctionComponents,
                    Solution>::type
                > {
    typedef detail::SearchObjFunctionComponentsToSearchComponents<
        SearchObjFunctionComponents, Solution> Convert;
    typedef typename Convert::type SearchComponents;
    typedef typename Convert::GainType Gain;
    typedef LocalSearchStep<
                Solution,
                SearchStrategy,
                SearchComponents> base;
    public:
    LocalSearchFunctionStep(Solution & sol, 
            SearchObjFunctionComponents s = SearchObjFunctionComponents()) :  
        base(sol, 
             SearchComponents
                   (
                    std::move(s.template get<GetNeighborhood>()),
                    Gain(std::move(s.template get<ObjFunction>()), base::m_searchComponents.template get<UpdateSolution>()),
                    std::move(s.template get<UpdateSolution>()),
                    std::move(s.template get<StopCondition>())
                   )
            ) {} 
};

template <typename SearchStrategy = search_strategies::ChooseFirstBetter,
          typename PostSearchAction,
          typename GlobalStopCondition,
          typename Solution,
          typename... Components>
bool local_search_obj_fun(
            Solution & solution,
            PostSearchAction psa,
            GlobalStopCondition gsc,
            Components... components) {
    LocalSearchFunctionStep<Solution, SearchStrategy, Components...> lss(solution, std::move(components)...);
    bool ret = false;
    while(!gsc(solution) && lss.search()) {
        ret = true;
        psa(solution);
    }
    return ret;
}

template <typename SearchStrategy = search_strategies::ChooseFirstBetter, 
          typename Solution, 
          typename... Components>
bool local_search_obj_fun_simple(Solution & solution, Components... components) {
    return local_search<SearchStrategy>(solution, utils::DoNothingFunctor(), utils::ReturnFalseFunctor(), std::move(components)...);
}


} //local_search
} //paal

#endif /* LOCAL_SEARCH_SINGLE_SOLUTION_OBJ_FUNCTION_HPP */
