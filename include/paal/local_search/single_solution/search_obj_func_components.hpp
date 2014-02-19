/**
 * @file search_obj_func_components.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-18
 */
#ifndef SEARCH_OCJ_FUNC_COMPONENTS_HPP
#define SEARCH_OCJ_FUNC_COMPONENTS_HPP

#include "paal/data_structures/components/components.hpp"
#include "paal/utils/functors.hpp"

namespace paal {
namespace local_search {

/**
 * @brief Name for GetMoves component
 */
struct GetMoves;
/**
 * @brief Name for ObjFunction component
 */
struct ObjFunction;
/**
 * @brief Name for Commit component
 */
struct Commit;

/**
 * @brief Components for objective function local search.
 *        This usually this class is not used. See SearchComponentsObjFun class.
 */
typedef data_structures::Components<
            GetMoves,
            ObjFunction,
            Commit
                > ComponentsObjFun;

/**
 * @brief SearchComponentsObjFun alias to components.
 *
 * @tparam Args
 */
template <typename... Args>
    using SearchComponentsObjFun = typename ComponentsObjFun::type<Args...> ;

/**
 * @brief make function for SearchComponentsObjFun
 *
 * @tparam Args
 * @param ComponentsObjFun::make_components(std::forward(args
 *
 * @return
 */
template <typename... Args>
auto make_SearchComponentsObjFun(Args&&... args) -> decltype(ComponentsObjFun::make_components(std::forward<Args>(args)...)) {
      return ComponentsObjFun::make_components(std::forward<Args>(args)...);
}

} //local_search
} //paal
#endif /* SEARCH_OCJ_FUNC_COMPONENTS_HPP */
