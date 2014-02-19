/**
 * @file component_traits.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-07-22
 */
#ifndef COMPONENT_TRAITS_HPP
#define COMPONENT_TRAITS_HPP
#include "components.hpp"
namespace paal {
namespace data_structures {

template <typename Components>
struct ComponentTraits;

template <typename Names, typename Types>
struct ComponentTraits<detail::Components<Names, Types>> {
    template <typename Name>
    using type = detail::TypeForName<Name, Names, Types>;
};

}
}
#endif /* COMPONENT_TRAITS_HPP */
