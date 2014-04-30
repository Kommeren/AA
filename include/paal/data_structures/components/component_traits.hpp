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

template <typename components>
struct component_traits;

template <typename Names, typename Types>
struct component_traits<detail::components<Names, Types>> {
    template <typename Name>
    using type = detail::type_for_name<Name, Names, Types>;
};

}
}
#endif /* COMPONENT_TRAITS_HPP */
