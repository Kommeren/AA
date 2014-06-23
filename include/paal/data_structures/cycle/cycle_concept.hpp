/**
 * @file cycle_concept.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-28
 */
#ifndef CYCLE_CONCEPT_HPP
#define CYCLE_CONCEPT_HPP

#include "paal/data_structures/cycle/cycle_traits.hpp"

#include <boost/concept_check.hpp>

namespace paal {
namespace data_structures {
namespace concepts {

template <typename X> class Cycle {
  public:
    BOOST_CONCEPT_USAGE(Cycle) {
        ve = x.vbegin();
        ve = x.vbegin(ce);
        ve = x.vend();
        x.flip(ce, ce);
    }

  private:
    X x;
    typename cycle_traits<X>::CycleElem ce;
    typename cycle_traits<X>::vertex_iterator ve;
};
}
}
}

#endif /* CYCLE_CONCEPT_HPP */
