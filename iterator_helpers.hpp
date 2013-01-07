/*
 *  * iterator_helpers.hpp
 *  *
 *  *  Created on: 03-01-2013
 *  *      Author: Piotr Wygocki
 *  */

/*
 *  UNFINISHED !!! MISING OPERATORS!!! 
 */


#include <functional>
 
template <typename Iterator, typename Cond> class IteratorWithExclusion : public Iterator {
    IteratorWithExclusion(Iterator i, Iterator end, const Cond & c) : m_i(i), m_end(end), m_c(c)  {}

    void operator++() {
        while(++m_i != m_end && c(*m_i)) {
            ++m_i;
        }
    }
    
    Iterator m_i;
    Iterator m_end;
    const Cond & m_c;
};
    
template <typename Iterator, typename Element> class IteratorWithExcludedElement : 
    public IteratorWithExclusion<Iterator, std::function<bool(Element)> > {
    
    IteratorWithExcludedElement(Iterator i, Iterator end, const Element & e) 
        : IteratorWithExclusion<Iterator, std::function<bool(Element)> >(i, end, std::bind(std::equal_to<Element>(), e, std::placeholders::_1) )  {}
};
