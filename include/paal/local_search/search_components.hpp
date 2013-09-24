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
        struct GetMoves;
        struct Gain;
        struct Commit;
        struct StopCondition;

        typedef data_structures::Components<
                    GetMoves, 
                    Gain, 
                    Commit, 
                    data_structures::NameWithDefault<StopCondition, utils::ReturnFalseFunctor>
                        > Components;

        template <typename... Args>
            using SearchComponents = typename Components::type<Args...> ;

        template <typename... Args>
            using MultiSearchComponents = SearchComponents<Args...> ;

        template <typename... Args>
        auto make_SearchComponents(Args&&... args) -> decltype(Components::make_components(std::forward<Args>(args)...)) {
              return Components::make_components(std::forward<Args>(args)...);
        }


    } //local_search
} //paal
#endif /* SEARCH_COMPONENTS_HPP */
