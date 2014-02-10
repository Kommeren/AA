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
#include "trivial_solution_commit.hpp"
#include "search_obj_func_components.hpp"

namespace paal {
namespace local_search {

/**
 * @brief traits class for SearchComponentsObjFun 
 *
 * @tparam SearchComponentsObjFun
 */
template <typename SearchComponentsObjFun> 
struct SearchObjFunctionComponentsTraits {
    typedef typename data_structures::ComponentTraits<SearchComponentsObjFun>::template type<GetMoves>::type GetMovesT; 
    typedef typename data_structures::ComponentTraits<SearchComponentsObjFun>::template type<ObjFunction>::type ObjFunctionT; 
    typedef typename data_structures::ComponentTraits<SearchComponentsObjFun>::template type<Commit>::type CommitT; 
};

namespace detail {
template <typename F, typename Solution, typename Commit> class FunToCheck {
        typedef decltype(std::declval<F>()(std::declval<Solution>())) Dist;
    public:
        FunToCheck(F f, const Commit & su) : m_f(std::move(f)), m_commitFunctor(su) {}

        template <typename Move> Dist operator()(const Solution &s , const Move &u) {
            Solution newS(s);
            m_commitFunctor(newS, u);
            return m_f(newS) - m_f(s);
        }

    private:

        F m_f;
        const Commit & m_commitFunctor;
};

template <typename SearchObjFunctionComponents, typename Solution>
class SearchObjFunctionComponentsToSearchComponents {
private:
    typedef SearchObjFunctionComponentsTraits<
                SearchObjFunctionComponents> traits; 
public:
    typedef detail::FunToCheck< 
                    typename traits::ObjFunctionT, 
                    Solution, 
                    typename traits::CommitT> GainType;
    typedef SearchComponents<
                typename traits::GetMovesT, 
                         GainType,
                typename traits::CommitT>  type;
};


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
                    std::move(s.template get<GetMoves>()),
                    Gain(std::move(s.template get<ObjFunction>()), base::m_searchComponents.template get<Commit>()),
                    std::move(s.template get<Commit>())
                   )
            ) {} 
};
} // !detail

/**
 * @brief local search function for objective function case.
 *
 * @tparam SearchStrategy
 * @tparam PostSearchAction
 * @tparam GlobalStopCondition
 * @tparam Solution
 * @tparam Components
 * @param solution
 * @param psa
 * @param gsc
 * @param components
 *
 * @return 
 */
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
    detail::LocalSearchFunctionStep<Solution, SearchStrategy, Components...> lss(solution, std::move(components)...);
    return search(lss, psa, gsc);
}

/**
 * @brief simple version of local_search_obj_fun
 *
 * @tparam SearchStrategy
 * @tparam Solution
 * @tparam Components
 * @param solution
 * @param components
 *
 * @return 
 */
template <typename SearchStrategy = search_strategies::ChooseFirstBetter, 
          typename Solution, 
          typename... Components>
bool local_search_obj_fun_simple(Solution & solution, Components... components) {
    return local_search<SearchStrategy>(solution, utils::SkipFunctor(), utils::ReturnFalseFunctor(), std::move(components)...);
}


} //local_search
} //paal

#endif /* LOCAL_SEARCH_SINGLE_SOLUTION_OBJ_FUNCTION_HPP */
