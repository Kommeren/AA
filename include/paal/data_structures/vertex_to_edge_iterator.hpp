/**
 * @file vertex_to_edge_iterator.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-20
 */
#ifndef VERTEX_TO_EDGE_ITERATOR_HPP
#define VERTEX_TO_EDGE_ITERATOR_HPP

#include "paal/utils/type_functions.hpp"

namespace paal {
namespace data_structures {

//TODO use boost:::iterator_fascade
/**
 * @class VertexToEdgeIterator
 * @brief transforms collection to collection of pairs consecutive elements of the input collection.
 *      The last element and the first element are considered consecutive.
 *
 * @tparam VertexIterator
 */
template <typename VertexIterator>
class VertexToEdgeIterator  {
public:
    typedef typename std::iterator_traits<VertexIterator>::value_type Vertex;
    typedef std::pair<Vertex, Vertex> Edge;

    typedef std::forward_iterator_tag iterator_category;
    typedef Edge                value_type;
    typedef ptrdiff_t           difference_type;
    typedef Edge *              pointer;
    typedef const Edge &        reference;

    /**
     * @brief constructor
     *
     * @param b
     * @param e
     */
    VertexToEdgeIterator(VertexIterator b, VertexIterator e) :
        m_idx(b), m_begin(b), m_end(e) {
            moveCurr();
        }

    VertexToEdgeIterator()  = default;

    /**
     * @brief operator++ post increment
     *
     * @return
     */
    VertexToEdgeIterator & operator++(){
        ++m_idx;
        moveCurr();

        return *this;
    }

    /**
     * @brief operator++ pre increment
     *
     * @return
     */
    VertexToEdgeIterator operator++(int){
        VertexToEdgeIterator i(*this);
        operator++();
        return i;
    }

    /**
     * @brief operator !=
     *
     * @param ei
     *
     * @return
     */
    bool operator!=(VertexToEdgeIterator ei) const {
        return !operator==(ei);
    }

    /**
     * @brief operator==
     *
     * @param ei
     *
     * @return
     */
    bool operator==(VertexToEdgeIterator ei) const {
        return m_idx == ei.m_idx;
    }

    /**
     * @brief operator->
     *
     * @return
     */
    const Edge * const operator->() const {
        return &m_curr;
    }


    /**
     * @brief operator*
     *
     * @return
     */
    const Edge & operator*() const {
        return m_curr;
    }

private:
    /**
     * @brief moves iterators to next position
     */
    void moveCurr() {
        if(m_idx != m_end) {
            m_curr.first = *m_idx;
            VertexIterator next = m_idx;
            ++next;
            if(next == m_end) {
                m_curr.second = *m_begin;
            } else {
                m_curr.second = *next;
            }
        }
    }

    VertexIterator m_idx;
    VertexIterator m_begin;
    VertexIterator m_end;
    Edge m_curr;
};

/**
 * @brief make for VertexToEdgeIterator
 *
 * @tparam VertexIterator
 * @param b
 * @param e
 *
 * @return
 */
template <typename VertexIterator>
VertexToEdgeIterator<VertexIterator>
make_VertexToEdgeIterator(VertexIterator b, VertexIterator e) {
    return VertexToEdgeIterator<VertexIterator>(b,e);
}

/**
 * @brief make for VertexToEdgeIterator form Vertex iterator pair
 *
 * @tparam VertexIterator
 * @param r
 *
 * @return
 */
template <typename VertexIterator>
VertexToEdgeIterator<VertexIterator>
make_VertexToEdgeIterator(std::pair<VertexIterator, VertexIterator> r) {
    return VertexToEdgeIterator<VertexIterator>(r.first, r.second);
}

}//data_structures
}//paal


#endif /* VERTEX_TO_EDGE_ITERATOR_HPP */
