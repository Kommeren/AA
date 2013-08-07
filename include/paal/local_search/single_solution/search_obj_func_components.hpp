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
#include "paal/utils/do_nothing_functor.hpp"

namespace paal {
namespace local_search {

struct GetMoves;
struct ObjFunction;
struct Commit;
struct StopCondition;
        
typedef data_structures::Components<
            GetMoves, 
            ObjFunction, 
            Commit, 
            data_structures::NameWithDefault<StopCondition, utils::ReturnFalseFunctor>
                > ComponentsObjFun;

template <typename... Args>
    using SearchComponentsObjFun = typename ComponentsObjFun::type<Args...> ;

template <typename... Args>
auto make_SearchComponentsObjFun(Args&&... args) -> decltype(ComponentsObjFun::make_components(std::forward<Args>(args)...)) {
      return ComponentsObjFun::make_components(std::forward<Args>(args)...);
}

} //local_search
} //paal
#endif /* SEARCH_OCJ_FUNC_COMPONENTS_HPP */
