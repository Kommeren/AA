/**
 * @file simple_cycle.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */

#ifndef SIMPLE_CYCLE_HPP
#define SIMPLE_CYCLE_HPP

#include <cassert>
#include <vector>
#include <iterator>

#include "paal/data_structures/bimap.hpp"

namespace paal {
namespace data_structures {


/**
 * @class SimpleCycle
 * @brief This is the simplest implementation of the \ref cycle concept based on the list.
 *
 * @tparam CycleEl
 * @tparam IdxT
 */
template <typename CycleEl, typename IdxT = int> class  SimpleCycle {
public:
    typedef std::pair<CycleEl, CycleEl> CycleElPair;
    typedef CycleEl CycleElement;

    /**
     * @brief constructor
     *
     * @tparam Iter
     * @param begin
     * @param end
     */
    template <typename Iter> SimpleCycle(Iter begin, Iter end) {
        if(begin == end) {
            return;
        }

        std::size_t size = std::distance(begin, end);

        m_predecessorMap.reserve(size);
        m_successorMap.reserve(size);

        IdxT prevIdx = add(*(begin++));
        IdxT firstIdx = prevIdx;
        for(;begin != end;++begin) {
            IdxT lastIdx =  add(*begin);
            link(prevIdx, lastIdx);
            prevIdx = lastIdx;
        }
        link(prevIdx, firstIdx);
    }

    ///after flip the order will be reversed, ie it will be from 'end'  to 'begin'
    void flip(const CycleEl & begin, const CycleEl & end) {
        IdxT e1 = toIdx(begin);
        IdxT b1 = prevIdx(e1);
        IdxT b2 = toIdx(end);
        IdxT e2 = nextIdx(b2);

        partialReverse(b2, e1);
        link(b1, b2);
        link(e1, e2);
    }

    /**
     * @brief number of elements in the cycle
     *
     * @return
     */
    std::size_t size() const {
        return  m_predecessorMap.size();
    }

    /**
     * @brief next element in the cycle
     *
     * @param ce
     *
     * @return
     */
    CycleEl next(const CycleEl & ce) const {
        return fromIdx(nextIdx(toIdx(ce)));
    }

    //TODO use iterator_fascade
    /**
     * @brief iterator over vertices of the cycle
     */
    class VertexIterator : public std::iterator<std::forward_iterator_tag, CycleEl,
                                ptrdiff_t, CycleEl *, const CycleEl &> {
        public:

            /**
             * @brief constructor
             *
             * @param cm
             * @param ce
             */
            VertexIterator(const SimpleCycle & cm, CycleEl ce ) :
                m_cycle(&cm), m_idx(m_cycle->toIdx(ce)), m_first(m_idx) {}

            /**
             * @brief default constructor
             */
            VertexIterator() : m_cycle(NULL) ,m_idx(-1) {}

            /**
             * @brief operator++()
             *
             * @return
             */
            VertexIterator & operator++(){
                m_idx = nextIdx(m_idx);

                if(m_idx == m_first) {
                    m_idx = -1;
                }

                return *this;
            }

            /**
             * @brief operator++(int)
             *
             * @return
             */
            VertexIterator operator++(int){
                VertexIterator i(*this);
                operator++();
                return i;
            }

            /**
             * @brief operator!=
             *
             * @param ei
             *
             * @return
             */
            bool operator!=(VertexIterator ei) const {
                return !operator==(ei);
            }

            /**
             * @brief operator==
             *
             * @param ei
             *
             * @return
             */
            bool operator==(VertexIterator ei) const {
                return m_idx == ei.m_idx;
            }

            /**
             * @brief operator->()
             *
             * @return
             */
            const CycleEl * const operator->() const {
               return &operator*();
            }

            /**
             * @brief operator*()
             *
             * @return
             */
            const CycleEl & operator*() const {
               return m_cycle->fromIdx(m_idx);
            }

        private:

            /**
             * @brief next element in the cycle
             *
             * @param i index of the element
             *
             * @return
             */
            IdxT nextIdx(IdxT i) const {
                return m_cycle->nextIdx(i);
            }

            const SimpleCycle * m_cycle;
            IdxT m_idx;
            IdxT m_first;
    };

    typedef std::pair<VertexIterator, VertexIterator> VertexIteratorRange;

    /**
     * @brief begin of the vertices range starting at el
     *
     * @param el
     *
     * @return
     */
    VertexIterator vbegin(const CycleEl & el) const {
        return VertexIterator(*this, el);
    }

    /**
     * @brief begin of the vertices range
     *
     * @return
     */
    VertexIterator vbegin() const {
        return vbegin(fromIdx(0));
    }

    /**
     * @brief end of the vertices range
     *
     * @return
     */
    VertexIterator vend() const {
        return VertexIterator();
    }

    /**
     * @brief returns range of vertices starting at el
     *
     * @param el
     *
     * @return
     */
    VertexIteratorRange getVerticesRange(const CycleEl & el) const {
        return VertexIteratorRange(vbegin(el), vend());
    }

    /**
     * @brief returns range of vertices
     *
     * @return
     */
    VertexIteratorRange getVerticesRange() const {
        return getVerticesRange(fromIdx(0));
    }

    //TODO use iterator_fascade
    /**
     * @brief Iterator on cycle edges
     */
    class EdgeIterator : public std::iterator<std::forward_iterator_tag, CycleElPair,
                                ptrdiff_t, CycleElPair *, const CycleElPair &> {
        public:
            /**
             * @brief constructor
             *
             * @param cm
             * @param ce
             */
            EdgeIterator(const SimpleCycle & cm, CycleEl ce ) :
                m_cycle(&cm), m_idx(m_cycle->toIdx(ce)), m_first(m_idx) {

                moveCurr();
            }

            /**
             * @brief default constructor
             */
            EdgeIterator() : m_cycle(NULL) ,m_idx(-1) {}

            /**
             * @brief operator++()
             *
             * @return
             */
            EdgeIterator & operator++(){
                m_idx = nextIdx(m_idx);
                moveCurr();

                if(m_idx == m_first) {
                    m_idx = -1;
                }

                return *this;
            }

            /**
             * @brief operator++(int)
             *
             * @return
             */
            EdgeIterator operator++(int){
                EdgeIterator i(*this);
                operator++();
                return i;
            }

            /**
             * @brief operator!=
             *
             * @param ei
             *
             * @return
             */
            bool operator!=(EdgeIterator ei) const {
                return !operator==(ei);
            }

            /**
             * @brief operator==
             *
             * @param ei
             *
             * @return
             */
            bool operator==(EdgeIterator ei) const {
                return m_idx == ei.m_idx;
            }

            /**
             * @brief operator->
             *
             * @return
             */
            const CycleElPair * const operator->() const {
                return &m_curr;
            }

            /**
             * @brief operator*()
             *
             * @return
             */
            const CycleElPair & operator*() const {
                return m_curr;
            }

        private:
            /**
             * @brief move to the next edge
             */
            void moveCurr() {
                m_curr.first = m_cycle->fromIdx(m_idx);
                m_curr.second = m_cycle->fromIdx(nextIdx(m_idx));
            }

            /**
             * @brief gets next id in the cycle
             *
             * @param i
             *
             * @return
             */
            IdxT nextIdx(IdxT i) const {
                return m_cycle->nextIdx(i);
            }

            const SimpleCycle * m_cycle;
            IdxT m_idx;
            IdxT m_first;
            CycleElPair m_curr;
    };

    typedef std::pair<EdgeIterator, EdgeIterator> EdgeIteratorRange;

    /**
     * @brief returns edges range starting at el
     *
     * @param el
     *
     * @return
     */
    EdgeIteratorRange getEdgeRange(const CycleEl & el) const {
        return EdgeIteratorRange(EdgeIterator(*this, el), EdgeIterator());
    }

    /**
     * @brief returns edges range
     *
     * @return
     */
    EdgeIteratorRange getEdgeRange() const {
        return getEdgeRange(fromIdx(0));
    }

protected:
    /**
     * @brief connects two vertices represented by ids
     *
     * @param x
     * @param y
     */
    void link(IdxT x, IdxT y) {
        m_successorMap[x] = y;
        m_predecessorMap[y] = x;
    }

    /**
     * @brief after this operation links from x to y are connected i reverse order
     *        after this function call cycle is in inconsistent state
     *
     * @param x
     * @param y
     */
    void partialReverse(IdxT x, IdxT y) {
        if(x == y)
            return;
        IdxT t_next = prevIdx(x);
        IdxT t;
        do {
            t = t_next;
            t_next = prevIdx(t);
            link(x,t);
            x = t;
        } while(t != y);
    }

    /**
     * @brief vertex to idx
     *
     * @param ce
     *
     * @return
     */
    IdxT toIdx(const CycleEl & ce) const {
        return m_cycleIdx.getIdx(ce);
    }

    /**
     * @brief returns next idx in the cycle
     *
     * @param i
     *
     * @return
     */
    IdxT nextIdx(IdxT i) const {
        return m_successorMap[i];
    }

    /**
     * @brief returns previous idx
     *
     * @param i
     *
     * @return
     */
    IdxT prevIdx(IdxT i) const {
        return m_predecessorMap[i];
    }

    /**
     * @brief idx to vertex
     *
     * @param i
     *
     * @return
     */
    const CycleEl & fromIdx(IdxT i) const {
        return m_cycleIdx.getVal(i);
    }

    /**
     * @brief ads new element to cycle data structures
     *
     * @param el
     *
     * @return
     */
    IdxT add(const CycleEl & el) {
        m_predecessorMap.push_back(-1);
        m_successorMap.push_back(-1);
        return m_cycleIdx.add(el);
    }

    ///mapping from elements to indexes
    BiMap<CycleEl, IdxT> m_cycleIdx;

    typedef std::vector<IdxT> SorsMap;

    ///predecessors
    SorsMap m_predecessorMap;
    ///successors
    SorsMap m_successorMap;
};

/**
 * @brief this class adapts Simple cycle to start from last changed position
 *
 * @tparam CycleEl
 * @tparam IdxT
 */
template <typename CycleEl, typename IdxT = int>
class  SimpleCycleStartFromLastChange : public SimpleCycle<CycleEl, IdxT> {
    typedef SimpleCycle<CycleEl, IdxT> base;
public:
    /**
     * @brief constructor
     *
     * @tparam Iter
     * @param b
     * @param e
     */
    template <typename Iter>
    SimpleCycleStartFromLastChange(Iter b, Iter e) :
        base(b,e), m_lastId(0) {}

    /**
     * @brief flip remembers last changed position
     *
     * @param begin
     * @param end
     */
    void flip(const CycleEl & begin, const CycleEl & end) {
        IdxT e1 = toIdx(begin);
        m_lastId = prevIdx(e1);
        base::flip(begin, end);
    }

    /**
     * @brief vbegin starts from last flip position
     *
     * @return
     */
    typename base::VertexIterator vbegin() const {
        return base::vbegin(fromIdx(m_lastId));
    }

private:
    IdxT m_lastId;
};



} //data_structures
} //paal

#endif /* SIMPLE_CYCLE_HPP */
