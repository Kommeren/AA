/**
 * @file local_search_obj_function.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-11
 */
#ifndef LOCAL_SEARCH_OBJ_FUNCTION_HPP
#define LOCAL_SEARCH_OBJ_FUNCTION_HPP

#include "local_search.hpp"
#include "trivial_solution_commit.hpp"
#include "search_obj_func_components.hpp"
#include "paal/data_structures/components/component_traits.hpp"

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
        FunToCheck(F f, const Commit & commit) : m_f(std::move(f)), m_commitFunctor(commit) {}

        template <typename Move> Dist operator()(const Solution &s , const Move &u) {
            Solution newS(s);
            m_commitFunctor(newS, u);
            return m_f(newS) - m_f(s);
        }

    private:

        F m_f;
        const Commit m_commitFunctor;
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

} // !detail


//TODO make it  variadic.
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
template <typename SearchStrategy,
          typename PostSearchAction,
          typename GlobalStopCondition,
          typename Solution,
          typename SearchObjFunctionComponents>
bool local_search_obj_fun(
            Solution & solution,
            SearchStrategy searchStrategy,
            PostSearchAction psa,
            GlobalStopCondition gsc,
            SearchObjFunctionComponents components) {
    typedef detail::SearchObjFunctionComponentsToSearchComponents<
        SearchObjFunctionComponents, Solution> Convert;

    typedef typename Convert::type SearchComponents;
    typedef typename Convert::GainType Gain;

    SearchComponents searchComponents{
                    std::move(components.template get<GetMoves>()),
                    Gain(std::move(components.template get<ObjFunction>()), components.template get<Commit>()),
                    std::move(components.template get<Commit>())};


    return local_search(solution, searchStrategy, psa, gsc, searchComponents);
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
template <typename SearchStrategy,
          typename Solution,
          typename Components>
bool local_search_obj_fun_simple(Solution & solution, Components components) {
    return local_search<SearchStrategy>(solution, ChooseFirstBetterStrategy{},
                utils::SkipFunctor(), utils::ReturnFalseFunctor(), std::move(components));
}


} //local_search
} //paal

#endif /* LOCAL_SEARCH_OBJ_FUNCTION_HPP */
