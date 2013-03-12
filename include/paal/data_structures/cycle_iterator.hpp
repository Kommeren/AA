/**
 * @file cycle_iterator.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-03-11
 */
#ifndef CYCLE_ITERATOR_HPP
#define CYCLE_ITERATOR_HPP 

#include "paal/utils/type_functions.hpp"

namespace paal {
namespace data_structures {

// could have been done by simple boost::join

/**
 * @class CycleIterator 
 * @brief represents Iterator to collection from start -> start.
 *        More precisely, start ... -> ... end -> begin .. -> ... start.
 *
 * @tparam Iter
 */
template <typename Iter>
class CycleIterator : public std::iterator_traits<Iter> {
    typedef std::iterator_traits<Iter> IT;
public:
    typedef typename IT::reference reference;
    typedef typename IT::pointer pointer;
    CycleIterator(Iter start, Iter begin, Iter end) : 
        m_curr(start), m_start(start), m_begin(begin), m_end(end) {}
    
    CycleIterator() {} 

    CycleIterator & operator++(){
        ++m_curr;

        if(m_curr == m_end) {
            m_curr = m_begin;
        }

        if(m_curr == m_start) {
            m_curr = m_end;    
        }

        return *this;
    }
    
    CycleIterator operator++(int){
        CycleIterator i(*this);
        operator++();
        return i;
    }

    bool operator!=(CycleIterator ei) const {
        return !operator==(ei);
    }               
    
    bool operator==(CycleIterator ei) const {
        return m_curr == ei.m_curr;
    }               
    
    pointer const operator->() const {
       return &operator*();
    }               


    reference operator*() const {
       return *m_curr;
    }
    
private:
    Iter m_curr;
    Iter m_start;
    Iter m_begin;
    Iter m_end;
};

}
}
#endif /* CYCLE_ITERATOR_HPP */
