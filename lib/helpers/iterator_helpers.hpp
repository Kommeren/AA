/**
 * @file iterator_helpers.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */

#include <boost/iterator/filter_iterator.hpp>
#include <boost/iterator/transform_iterator.hpp>

#include "type_functions.hpp"

#ifndef __ITERATOR_HELPERS__
#define __ITERATOR_HELPERS__

namespace paal {
namespace helpers {

template <typename Iterator> class IteratorWithExcludedElement : 
    public boost::filter_iterator<std::function<bool(const typename IterToElem<Iterator>::type &)>, Iterator> {
public:
    typedef typename IterToElem<Iterator>::type Element;
    IteratorWithExcludedElement(Iterator i, Iterator end, const Element &  e) 
        : boost::filter_iterator<std::function<bool(const Element &)>, Iterator >
          (std::bind(std::not_equal_to<Element>(), e, std::placeholders::_1), i, end )  {}
    
    IteratorWithExcludedElement() {} 
};


template <typename Iterator,int k> class SubsetsIterator : 
    private SubsetsIterator<Iterator, k-1>{
    
public:
    typedef typename IterToElem<Iterator>::type Element;
    typedef typename kTuple<Element, k>::type SubsetType; 
    typedef SubsetsIterator<Iterator, k-1> base;
    typedef std::iterator<std::forward_iterator_tag, 
                         SubsetType,
                         ptrdiff_t,
                         SubsetType *,
                         const SubsetType &> IterBase;

    //couldn't be done by inheritance from itarator
    typedef typename IterBase::iterator_category iterator_category;
    typedef typename IterBase::value_type        value_type;
    typedef typename IterBase::difference_type   difference_type;
    typedef typename IterBase::pointer           pointer;
    typedef typename IterBase::reference         reference;
    using base::m_end;


    SubsetsIterator(Iterator begin, Iterator end ) : 
        base(begin, end), m_begin(base::m_begin) {
        ++m_begin;

        updateCurr();
    }
    
    SubsetsIterator()  {}

    SubsetsIterator & operator++(){
        ++m_begin;
        while(m_begin == m_end) {
            base::operator++();
            m_begin = base::m_begin;
            ++m_begin;
        }
        updateCurr();

        return *this;
    }
    
    SubsetsIterator operator++(int){
        SubsetsIterator i(*this);
        operator++();
        return i;
    }

    bool operator!=(SubsetsIterator ei) const {
        return !operator==(ei);
    }               
    
    bool operator==(const SubsetsIterator & ei) const {
        return (m_begin == ei.m_begin && base::operator==(ei)) ||
               (m_begin == m_end && ei.m_begin == m_end);
    }               
    
    const SubsetType * const operator->() const {
        return &m_return;
    }               

    void operator=(SubsetsIterator ei) {
        base::operator=(ei);
        m_begin = ei.m_begin; 
        m_end = ei.m_end;
        m_return = ei.m_return;
    }               

    const SubsetType & operator*() const {
        return m_return;
    }
    
private:
    void updateCurr() {
        if(m_begin != m_end) {
            m_return = std::tuple_cat(base::operator*(), std::tuple<Element>(*m_begin));
        }
    }
    
    SubsetType m_return;
protected:    
    Iterator m_begin;
};

template <typename Iterator> class SubsetsIterator<Iterator, 1> : 
    public std::iterator<std::forward_iterator_tag, 
                         std::pair<std::tuple<typename IterToElem<Iterator>::type>, Iterator>,
                         ptrdiff_t, 
                         std::pair<std::tuple<typename IterToElem<Iterator>::type>, Iterator> *,
                         const std::pair<std::tuple<typename IterToElem<Iterator>::type>, Iterator> &> {
public:    
    typedef typename IterToElem<Iterator>::type Element;
    typedef std::tuple<Element> SubsetType;

    SubsetsIterator(Iterator begin, Iterator end ) : 
        m_begin(begin), m_end(end) {

        updateCurr();
    }
    
    SubsetsIterator()  {}

    SubsetsIterator & operator++(){
        ++m_begin;
        updateCurr();

        return *this;
    }
    
    SubsetsIterator operator++(int){
        SubsetsIterator i(*this);
        operator++();
        return i;
    }

    bool operator!=(SubsetsIterator ei) const {
        return !operator==(ei);
    }               
    
    bool operator==(SubsetsIterator ei) const {
        return m_begin == ei.m_begin;
    }               
    
    const SubsetType * const operator->() const {
        return &m_return;
    }               

    void operator=(SubsetsIterator ei) {
        m_begin = ei.m_begin; 
        m_end = ei.m_end;
        m_return = ei.m_return;
    }               

    const SubsetType & operator*() const {
        return m_return;
    }
    
private:
    void updateCurr() {
        if(m_begin != m_end) {
            m_return = SubsetType(*m_begin);
        }
    }
    SubsetType m_return;
protected:    
    Iterator m_begin;
    Iterator m_end;
};

} //helpers
} //paal

#endif // __ITERATOR_HELPERS__

