/**
 * @file paal/local_search/search_components.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-18
 */
#ifndef SEARCH_COMPONENTS_HPP
#define SEARCH_COMPONENTS_HPP 

#include "paal/data_structures/components/components.hpp"
#include "paal/utils/functors.hpp"

namespace paal {
namespace local_search {


    /**
     * @brief name for the GetMoves component
     */
    struct GetMoves;
    /**
     * @brief name for the Gain component
     */
    struct Gain;
    /**
     * @brief name for the Commit component
     */
    struct Commit;

    /**
     * @brief Definition for the Components class for local search
     * usually this class is not directly used, see SearchComponents.
     */
    typedef data_structures::Components<
                GetMoves, 
                Gain, 
                Commit 
                    > Components;

    /**
     * @brief SearchComponents template alias
     *
     * @tparam Args
     */
    template <typename... Args>
        using SearchComponents = typename Components::type<Args...> ;

    /**
     * @brief MultiSearchComponents template alias
     *
     * @tparam Args
     */
    template <typename... Args>
        using MultiSearchComponents = SearchComponents<Args...> ;

    /**
     * @brief make function for search components
     *
     * @tparam Args
     *
     * @return SearchComponents or MultiSearchComponents 
     */
    template <typename... Args>
    auto make_SearchComponents(Args&&... args) -> decltype(Components::make_components(std::forward<Args>(args)...)) {
          return Components::make_components(std::forward<Args>(args)...);
    }


} //local_search
} //paal
#endif /* SEARCH_COMPONENTS_HPP */
