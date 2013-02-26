/**
 * @file cycle_start_from_last_change.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-26
 */
#ifndef CYCLE_START_FROM_LAST_CHANGE_HPP
#define CYCLE_START_FROM_LAST_CHANGE_HPP 

#include "cycle_traits.hpp"

namespace paal {
namespace data_structures {

template <typename Cycle> 
class CycleStartFromLastChange {
    typedef typename  CycleTraits<Cycle>::CycleElem CycleElem;
    typedef typename  CycleTraits<Cycle>::VertexIterator VertexIterator;
public:
    CycleStartFromLastChange(Cycle & c) :
        m_cycle(c), m_element(*c.vbegin()) {}

    void flip(const CycleElem & begin, const CycleElem & end) {
        m_element = end;
        m_cycle.flip(begin, end);
    }
    
    VertexIterator vbegin() const {
        return m_cycle.vbegin(m_element);
    }
    
    VertexIterator vbegin(const CycleElem & ce) const {
        return m_cycle.vbegin(ce);
    }

    VertexIterator vend() const {
        return m_cycle.vend();
    }

    Cycle & getCycle() {
        return m_cycle;
    }

private:
    Cycle & m_cycle;
    CycleElem m_element;
};

}
}

    
#endif /* CYCLE_START_FROM_LAST_CHANGE_HPP */
