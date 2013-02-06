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


namespace detail {
template <typename Iterator,int k> class SubsetsIteratorHelper : 
    public std::iterator<std::forward_iterator_tag, 
                         std::pair<typename kTuple<typename IterToElem<Iterator>::type, k>::type, Iterator>,
                         ptrdiff_t,
                         std::pair<typename kTuple<typename IterToElem<Iterator>::type, k>::type, Iterator> *,
                         const std::pair<typename kTuple<typename IterToElem<Iterator>::type, k>::type, Iterator> &>,
    private SubsetsIteratorHelper<Iterator, k-1>{
    
public:
    typedef typename IterToElem<Iterator>::type Element;
    typedef typename kTuple<Element, k>::type SubsetType; 
    typedef std::pair<SubsetType, Iterator> ReturnType;
    typedef SubsetsIteratorHelper<Iterator, k-1> base;
    typedef std::iterator<std::forward_iterator_tag, 
                         ReturnType,
                         ptrdiff_t,
                         ReturnType *,
                         const ReturnType &> IterBase;

    //this is horrible!!!
    typedef typename IterBase::iterator_category iterator_category;
    typedef typename IterBase::value_type        value_type;
    typedef typename IterBase::difference_type   difference_type;
    typedef typename IterBase::pointer           pointer;
    typedef typename IterBase::reference         reference;
    using base::m_end;


    SubsetsIteratorHelper(Iterator begin, Iterator end ) : 
        base(begin, end), m_begin(base::m_begin) {
        ++m_begin;

        updateCurr();
    }
    
    SubsetsIteratorHelper()  {}

    SubsetsIteratorHelper & operator++(){
        ++m_begin;
        while(m_begin == m_end) {
            base::operator++();
            m_begin = base::m_begin;
            ++m_begin;
        }
        updateCurr();

        return *this;
    }
    
    SubsetsIteratorHelper operator++(int){
        SubsetsIteratorHelper i(*this);
        operator++();
        return i;
    }

    bool operator!=(SubsetsIteratorHelper ei) const {
        return !operator==(ei);
    }               
    
    bool operator==(const SubsetsIteratorHelper & ei) const {
        return (m_begin == ei.m_begin && base::operator==(ei)) ||
               (m_begin == m_end && ei.m_begin == m_end);
    }               
    
    const ReturnType * const operator->() const {
        return &m_return;
    }               

    void operator=(SubsetsIteratorHelper ei) {
        base::operator=(ei);
        m_begin = ei.m_begin; 
        m_end = ei.m_end;
        m_return = ei.m_return;
    }               

    const ReturnType & operator*() const {
        return m_return;
    }
    
private:
    void updateCurr() {
        if(m_begin != m_end) {
            m_return = ReturnType(std::tuple_cat(base::operator*().first, std::tuple<Element>(*m_begin)), m_begin);
        }
    }
    
    ReturnType m_return;
protected:    
    Iterator m_begin;
};

template <typename Iterator> class SubsetsIteratorHelper<Iterator, 1> : 
    public std::iterator<std::forward_iterator_tag, 
                         std::pair<std::tuple<typename IterToElem<Iterator>::type>, Iterator>,
                         ptrdiff_t, 
                         std::pair<std::tuple<typename IterToElem<Iterator>::type>, Iterator> *,
                         const std::pair<std::tuple<typename IterToElem<Iterator>::type>, Iterator> &> {
public:    
    typedef typename IterToElem<Iterator>::type Element;
    typedef std::tuple<Element> SubsetType;
    typedef std::pair<SubsetType, Iterator> ReturnType;

    SubsetsIteratorHelper(Iterator begin, Iterator end ) : 
        m_begin(begin), m_end(end) {

        updateCurr();
    }
    
    SubsetsIteratorHelper()  {}

    SubsetsIteratorHelper & operator++(){
        ++m_begin;
        updateCurr();

        return *this;
    }
    
    SubsetsIteratorHelper operator++(int){
        SubsetsIteratorHelper i(*this);
        operator++();
        return i;
    }

    bool operator!=(SubsetsIteratorHelper ei) const {
        return !operator==(ei);
    }               
    
    bool operator==(SubsetsIteratorHelper ei) const {
        return m_begin == ei.m_begin;
    }               
    
    const ReturnType * const operator->() const {
        return &m_return;
    }               

    void operator=(SubsetsIteratorHelper ei) {
        m_begin = ei.m_begin; 
        m_end = ei.m_end;
        m_return = ei.m_return;
    }               

    const ReturnType & operator*() const {
        return m_return;
    }
    
private:
    void updateCurr() {
        if(m_begin != m_end) {
            m_return = ReturnType(SubsetType(*m_begin), m_begin);
        }
    }
    ReturnType m_return;
protected:    
    Iterator m_begin;
    Iterator m_end;
};

template <typename Pair> struct GetFirst{
    auto operator()(const Pair & p) const -> const decltype(p.first) & {
        return p.first;
    }
};
}

//minor TODO it can be done more efficient in direct way 
template <typename Iterator, int k> class SubsetsIterator : 
    public boost::transform_iterator< detail::GetFirst < std::pair<typename kTuple<typename IterToElem<Iterator>::type, k>::type, Iterator> >,
                                      detail::SubsetsIteratorHelper<Iterator, k>,
                                      const typename kTuple<typename IterToElem<Iterator>::type, k>::type &> {

    typedef typename IterToElem<Iterator>::type Element;
    typedef typename kTuple<Element, k>::type KTuple;
    typedef std::pair<KTuple, Iterator> HelperResPair;
    typedef detail::SubsetsIteratorHelper<Iterator, k> Helper;
    typedef boost::transform_iterator< detail::GetFirst <HelperResPair>,
                                      Helper,
                                      const KTuple &> Trans;
public:
    SubsetsIterator(Iterator begin, Iterator end) :Trans(Helper(begin, end), detail::GetFirst<HelperResPair>()) {}
};


} //helpers
} //paal

#endif // __ITERATOR_HELPERS__

