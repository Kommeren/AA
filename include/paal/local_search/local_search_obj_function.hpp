//=======================================================================
// Copyright (c) 2013 Piotr Wygocki
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file local_search_obj_function.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-11
 */
#ifndef PAAL_LOCAL_SEARCH_OBJ_FUNCTION_HPP
#define PAAL_LOCAL_SEARCH_OBJ_FUNCTION_HPP

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
template <typename SearchComponentsObjFun>
struct search_obj_function_components_traits {
    using get_moves_t = typename data_structures::component_traits<
        SearchComponentsObjFun>::template type<get_moves>::type;
    using obj_function_t = typename data_structures::component_traits<
        SearchComponentsObjFun>::template type<ObjFunction>::type;
    using commit_t = typename data_structures::component_traits<
        SearchComponentsObjFun>::template type<Commit>::type;
};

namespace detail {

template <typename F, typename Commit>
struct fun_to_check {

    fun_to_check(F f, const Commit &commit)
        : m_f(std::move(f)), m_commit_functor(commit) {}

    template <typename Solution, typename Move> auto operator()(const Solution &s, const Move &u) {
        Solution newS(s);
        m_commit_functor(newS, u);
        return m_f(newS) - m_f(s);
    }

private:

    F m_f;
    const Commit m_commit_functor;
};

template <typename SearchObjFunctionComponents>
class search_obj_function_components_to_search_components {
    using traits =  search_obj_function_components_traits<SearchObjFunctionComponents>;

public:
    using gain_type = detail::fun_to_check<typename traits::obj_function_t,
                                 typename traits::commit_t>;
    using type = search_components<typename traits::get_moves_t, gain_type,
                              typename traits::commit_t>;
};

template <typename SearchObjFunctionComponents>
auto convert_comps(SearchObjFunctionComponents components) {

    using convert =  detail::search_obj_function_components_to_search_components<
        SearchObjFunctionComponents>;

    using search_components = typename convert::type;
    using gain =  typename convert::gain_type;

    return search_components{
        std::move(components.template get<get_moves>()),
        gain(std::move(components.template get<ObjFunction>()),
             components.template get<Commit>()),
        std::move(components.template get<Commit>())
    };
}

} // !detail

/**
 * @brief local search function for objective function case.
 *
 * @tparam SearchStrategy
 * @tparam ContinueOnSuccess
 * @tparam ContinueOnFail
 * @tparam Solution
 * @tparam SearchObjFunctioncomponents
 * @param solution
 * @param searchStrategy
 * @param on_success
 * @param on_fail
 * @param components
 *
 * @return
 */
template <typename SearchStrategy, typename ContinueOnSuccess,
          typename ContinueOnFail, typename Solution,
          typename... SearchObjFunctionComponents>
bool local_search_obj_fun(Solution &solution, SearchStrategy searchStrategy,
                          ContinueOnSuccess on_success, ContinueOnFail on_fail,
                          SearchObjFunctionComponents ... components) {

    return local_search(solution, searchStrategy, std::move(on_success),
                        std::move(on_fail), detail::convert_comps(std::move(components))...);
}

/**
 * @brief simple version of local_search_obj_fun
 *
 * @tparam SearchStrategy
 * @tparam Solution
 * @tparam Components
 * @param solution
 * @param comps
 *
 * @return
 */
template <typename Solution, typename... Components>
bool obj_fun_first_improving(Solution &solution, Components... comps) {
    return local_search_obj_fun(solution, first_improving_strategy{},
                                utils::always_true{}, utils::always_false{},
                                std::move(comps)...);
}

/**
 * @brief simple version of local_search_obj_fun
 *
 * @tparam Solution
 * @tparam Components
 * @param solution
 * @param comps
 *
 * @return
 */
template <typename Solution, typename... Components>
bool obj_fun_best_improving(Solution &solution, Components... comps) {
    return local_search_obj_fun(solution, best_improving_strategy{},
                                utils::always_true{}, utils::always_false{},
                                std::move(comps)...);
}

} // local_search
} // paal

#endif // PAAL_LOCAL_SEARCH_OBJ_FUNCTION_HPP
