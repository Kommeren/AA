/**
 * @file VertexToEdngeIterator.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-20
 */
#ifndef VERTEXTOEDNGEITERATOR_HPP
#define VERTEXTOEDNGEITERATOR_HPP 

#include "paal/helpers/type_functions.hpp"

namespace  paal {
namespace helpers {

template <typename VertexIterator>
class VertexToEdgeIterator  {
public:
    typedef typename IterToElem<VertexIterator>::type Vertex;
    typedef std::pair<Vertex, Vertex> Edge;

    typedef std::forward_iterator_tag iterator_category;
    typedef Edge                value_type;
    typedef ptrdiff_t           difference_type;
    typedef Edge *              pointer;
    typedef const Edge &        reference;

    VertexToEdgeIterator(VertexIterator b, VertexIterator e) : 
        m_idx(b), m_begin(b), m_end(e) {
            updateCurr();
        }

    VertexToEdgeIterator()  {}

    VertexToEdgeIterator & operator++(){
        ++m_idx;
        updateCurr();

        return *this;
    }

    VertexToEdgeIterator operator++(int){
        VertexToEdgeIterator i(*this);
        operator++();
        return i;
    }

    bool operator!=(VertexToEdgeIterator ei) const {
        return !operator==(ei);
    }               

    bool operator==(VertexToEdgeIterator ei) const {
        return m_idx == ei.m_idx;
    }               

    const Edge * const operator->() const {
        return &m_curr;
    }               

    void operator=(VertexToEdgeIterator ei) {
        m_idx = ei.m_idx; 
        m_begin = ei.m_begin;
        m_curr = ei.m_curr;
        m_end = ei.m_end;
    }               

    const Edge & operator*() const {
        return m_curr;
    }

private:
    void updateCurr() {
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

template <typename VertexIterator>
VertexToEdgeIterator<VertexIterator>
make_VertexToEdgeIterator(VertexIterator b, VertexIterator e) {
    return VertexToEdgeIterator<VertexIterator>(b,e);
}

template <typename VertexIterator>
VertexToEdgeIterator<VertexIterator>
make_VertexToEdgeIterator(std::pair<VertexIterator, VertexIterator> r) {
    return VertexToEdgeIterator<VertexIterator>(r.first, r.second);
}

}//paal
}//helpers


#endif /* VERTEXTOEDNGEITERATOR_HPP */
