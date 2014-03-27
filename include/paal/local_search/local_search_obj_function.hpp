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
 * @brief traits class for search_componentsObjFun
 *
 * @tparam search_componentsObjFun
 */
template <typename search_componentsObjFun>
struct search_obj_function_components_traits {
    typedef typename data_structures::component_traits<search_componentsObjFun>::template type<get_moves>::type get_movesT;
    typedef typename data_structures::component_traits<search_componentsObjFun>::template type<ObjFunction>::type ObjFunctionT;
    typedef typename data_structures::component_traits<search_componentsObjFun>::template type<Commit>::type CommitT;
};

namespace detail {

template <typename F, typename Solution, typename Commit> class fun_to_check {
        typedef decltype(std::declval<F>()(std::declval<Solution>())) Dist;
    public:
        fun_to_check(F f, const Commit & commit) : m_f(std::move(f)), m_commit_functor(commit) {}

        template <typename Move> Dist operator()(const Solution &s , const Move &u) {
            Solution newS(s);
            m_commit_functor(newS, u);
            return m_f(newS) - m_f(s);
        }

    private:

        F m_f;
        const Commit m_commit_functor;
};

template <typename SearchObjFunctioncomponents, typename Solution>
class search_obj_function_components_tosearch_components {
private:
    typedef search_obj_function_components_traits<
                SearchObjFunctioncomponents> traits;
public:
    typedef detail::fun_to_check<
                    typename traits::ObjFunctionT,
                    Solution,
                    typename traits::CommitT> GainType;
    typedef search_components<
                typename traits::get_movesT,
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
 * @tparam components
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
          typename SearchObjFunctioncomponents>
bool local_search_obj_fun(
            Solution & solution,
            SearchStrategy searchStrategy,
            PostSearchAction psa,
            GlobalStopCondition gsc,
            SearchObjFunctioncomponents components) {
    typedef detail::search_obj_function_components_tosearch_components<
        SearchObjFunctioncomponents, Solution> Convert;

    typedef typename Convert::type search_components;
    typedef typename Convert::GainType Gain;

    search_components searchcomponents{
                    std::move(components.template get<get_moves>()),
                    Gain(std::move(components.template get<ObjFunction>()), components.template get<Commit>()),
                    std::move(components.template get<Commit>())};


    return local_search(solution, searchStrategy, psa, gsc, searchcomponents);
}

/**
 * @brief simple version of local_search_obj_fun
 *
 * @tparam SearchStrategy
 * @tparam Solution
 * @tparam components
 * @param solution
 * @param components
 *
 * @return
 */
template <typename SearchStrategy,
          typename Solution,
          typename components>
bool local_search_obj_fun_simple(Solution & solution, components comps) {
    return local_search<SearchStrategy>(solution, choose_first_better_strategy{},
                utils::skip_functor(), utils::return_false_functor(), std::move(comps));
}


} //local_search
} //paal

#endif /* LOCAL_SEARCH_OBJ_FUNCTION_HPP */
