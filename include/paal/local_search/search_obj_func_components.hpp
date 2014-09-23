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
#include "paal/local_search/search_components.hpp"
#include "paal/utils/functors.hpp"

namespace paal {
namespace local_search {

/**
 * @brief Name for get_moves component
 */
struct get_moves;
/**
 * @brief Name for ObjFunction component
 */
struct ObjFunction;
/**
 * @brief Name for Commit component
 */
struct Commit;

/**
 * @brief components for objective function local search.
 *        This usually this class is not used. See search_componentsObjFun
 * class.
 */
typedef data_structures::components<get_moves, ObjFunction, Commit>
    componentsObjFun;

/**
 * @brief search_componentsObjFun alias to components.
 *
 * @tparam Args
 */
template <typename... Args>
using search_componentsObjFun = typename componentsObjFun::type<Args...>;

/**
 * @brief make function for search_componentsObjFun
 *
 * @tparam Args
 * @param args
 *
 * @return
 */
template <typename... Args>
auto make_search_componentsObjFun(Args &&... args) {
    return componentsObjFun::make_components(std::forward<Args>(args)...);
}

} //! local_search
} //! paal
#endif /* SEARCH_OCJ_FUNC_COMPONENTS_HPP */
