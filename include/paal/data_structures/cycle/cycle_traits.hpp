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

template <typename Cycle>
struct CycleTraits {
    typedef decltype(std::declval<Cycle>().vbegin()) VertexIterator;
    typedef typename std::iterator_traits<VertexIterator>::value_type CycleElem;
};

}
}
#endif /* CYCLE_TRAITS_HPP */
