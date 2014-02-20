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

    /**
     * @brief adopts any cycle to start (vbegin) i place of the last change(flip)
     *
     * @tparam Cycle
     */
template <typename Cycle>
class CycleStartFromLastChange {
public:
    typedef typename  CycleTraits<Cycle>::CycleElem CycleElem;
    typedef typename  CycleTraits<Cycle>::VertexIterator VertexIterator;


    /**
     * @brief constructor
     *
     * @param c
     */
    CycleStartFromLastChange(Cycle & c) :
        m_cycle(c), m_element(*c.vbegin()) {}

    /**
     * @brief flip stores place of this flip
     *
     * @param begin
     * @param end
     */
    void flip(const CycleElem & begin, const CycleElem & end) {
        m_element = end;
        m_cycle.flip(begin, end);
    }

    /**
     * @brief vbegin starts from last flip
     *
     * @return
     */
    VertexIterator vbegin() const {
        return m_cycle.vbegin(m_element);
    }

    /**
     * @brief vbegin starts from ce
     *
     * @param ce
     *
     * @return
     */
    VertexIterator vbegin(const CycleElem & ce) const {
        return m_cycle.vbegin(ce);
    }

    /**
     * @brief vertices end
     *
     * @return
     */
    VertexIterator vend() const {
        return m_cycle.vend();
    }

    /**
     * @brief cycle getter
     *
     * @return
     */
    Cycle & getCycle() {
        return m_cycle;
    }

    /**
     * @brief cycle getter const version
     *
     * @return
     */
    const Cycle & getCycle() const {
        return m_cycle;
    }

private:
    Cycle & m_cycle;
    CycleElem m_element;
};

}
}


#endif /* CYCLE_START_FROM_LAST_CHANGE_HPP */
