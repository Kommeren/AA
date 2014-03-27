/**
 * @file cycle_traits.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-26
 */
#ifndef CYCLE_TRAITS_HPP
#define CYCLE_TRAITS_HPP

#include "paal/utils/type_functions.hpp"

namespace paal {
namespace data_structures {


    /**
     * @brief traits for \ref cycle concept
     *
     * @tparam Cycle
     */
template <typename Cycle>
struct cycle_traits {
    ///Vertex iterator type
    typedef decltype(std::declval<Cycle>().vbegin()) vertex_iterator;
    typedef typename std::iterator_traits<vertex_iterator>::value_type CycleElem;
};

}
}
#endif /* CYCLE_TRAITS_HPP */
