/**
 * @file subset_iterator.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */

#include <boost/iterator/filter_iterator.hpp>
#include <boost/iterator/transform_iterator.hpp>

#include "paal/utils/type_functions.hpp"

#ifndef SUBSET_ITERATOR_HPP
#define SUBSET_ITERATOR_HPP

namespace paal {
namespace data_structures {

//minor TODO could be more efficient in direct array of iterators implementation
//TODO use boost::iterator_fascade
/**
 * @class SubsetsIterator
 * @brief Iterator to all k-subsets of given collection.
 *
 * @tparam Iterator
 * @tparam k
 */
template <typename Iterator,int k,
          typename Pointer = typename utils::kTuple<typename std::iterator_traits<Iterator>::value_type, k>::type *,
          typename Reference = typename utils::kTuple<typename std::iterator_traits<Iterator>::value_type, k>::type const &> class SubsetsIterator :
    private SubsetsIterator<Iterator, k-1>{

public:
    typedef typename std::iterator_traits<Iterator>::value_type Element;
    typedef typename utils::kTuple<Element, k>::type SubsetType;
    typedef SubsetsIterator<Iterator, k-1> base;
    typedef std::iterator<std::forward_iterator_tag,
                         SubsetType,
                         ptrdiff_t,
                         Pointer,
                         Reference> IterBase;

    //couldn't be done by inheritance from iterator
    typedef typename IterBase::iterator_category iterator_category;
    typedef typename IterBase::value_type        value_type;
    typedef typename IterBase::difference_type   difference_type;
    typedef typename IterBase::pointer           pointer;
    typedef typename IterBase::reference         reference;
    using base::m_end;


    /**
     * @brief constructor
     *
     * @param begin
     * @param end
     */
    SubsetsIterator(Iterator begin, Iterator end ) :
        base(begin, end), m_begin(base::m_begin) {
        if(m_begin != m_end) {
            ++m_begin;
        }

        moveCurr();
    }

    SubsetsIterator()  = default;

    /**
     * @brief operator++
     *
     * @return
     */
    SubsetsIterator & operator++(){
        ++m_begin;
        while(m_begin == m_end) {
            base::operator++();
            m_begin = base::m_begin;
            if(m_begin == m_end) {
                return *this;
            }
            ++m_begin;
        }
        moveCurr();

        return *this;
    }

    /**
     * @brief operator++(int)
     *
     * @return
     */
    SubsetsIterator operator++(int){
        SubsetsIterator i(*this);
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
    bool operator!=(SubsetsIterator ei) const {
        return !operator==(ei);
    }

    /**
     * @brief operator==
     *
     * @param ei
     *
     * @return
     */
    bool operator==(const SubsetsIterator & ei) const {
        return (m_begin == ei.m_begin && base::operator==(ei)) ||
               (m_begin == m_end && ei.m_begin == m_end);
    }

    /**
     * @brief operator->
     *
     * @return
     */
    pointer const operator->() const {
        return &m_return;
    }

    /**
     * @brief operator*
     *
     * @return
     */
    reference operator*() const {
        return m_return;
    }

private:
    /**
     * @brief sets m_return to new value
     */
    void moveCurr() {
        if(m_begin != m_end) {
            m_return = std::tuple_cat(base::operator*(), std::tuple<Element>(*m_begin));
        }
    }

    SubsetType m_return;
protected:
    ///begin of collection
    Iterator m_begin;
};

/**
 * @brief specialization for k = 1
 *
 * @tparam Iterator
 */
template <typename Iterator> class SubsetsIterator<Iterator, 1> :
    public std::iterator<std::forward_iterator_tag,
                         std::pair<std::tuple<typename std::iterator_traits<Iterator>::value_type>, Iterator>,
                         ptrdiff_t,
                         std::pair<std::tuple<typename std::iterator_traits<Iterator>::value_type>, Iterator> *,
                         const std::pair<std::tuple<typename std::iterator_traits<Iterator>::value_type>, Iterator> &> {
public:
    typedef typename std::iterator_traits<Iterator>::value_type Element;
    typedef std::tuple<Element> SubsetType;

    /**
     * @brief constructor
     *
     * @param begin
     * @param end
     */
    SubsetsIterator(Iterator begin, Iterator end ) :
        m_begin(begin), m_end(end) {

        moveCurr();
    }

    SubsetsIterator() = default;

    /**
     * @brief operator++()
     *
     * @return
     */
    SubsetsIterator & operator++(){
        assert(m_begin != m_end);
        ++m_begin;

        moveCurr();

        return *this;
    }

    /**
     * @brief operator++(int)
     *
     * @return
     */
    SubsetsIterator operator++(int){
        SubsetsIterator i(*this);
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
    bool operator!=(SubsetsIterator ei) const {
        return !operator==(ei);
    }

    /**
     * @brief operator==
     *
     * @param ei
     *
     * @return
     */
    bool operator==(SubsetsIterator ei) const {
        return m_begin == ei.m_begin;
    }

    /**
     * @brief operator->
     *
     * @return
     */
    const SubsetType * const operator->() const {
        return &m_return;
    }

    /**
     * @brief operator*
     *
     * @return
     */
    const SubsetType & operator*() const {
        return m_return;
    }

private:
    /**
     * @brief sets m_return
     */
    void moveCurr() {
        if(m_begin != m_end) {
            m_return = SubsetType(*m_begin);
        }
    }
    SubsetType m_return;
protected:
    /// begin of the collection
    Iterator m_begin;
    /// end of the collection
    Iterator m_end;
};


/**
 * @brief make for SubsetsIterator
 *
 * @tparam Iterator
 * @tparam k
 * @tparam Pointer
 * @param b
 * @param e
 *
 * @return
 */
template <typename Iterator,int k,
          typename Pointer = typename utils::kTuple<typename std::iterator_traits<Iterator>::value_type, k>::type *,
          typename Reference = typename utils::kTuple<typename std::iterator_traits<Iterator>::value_type, k>::type const &>
std::pair<SubsetsIterator<Iterator, k, Pointer, Reference>, SubsetsIterator<Iterator, k, Pointer, Reference>>
make_SubsetsIteratorrange(Iterator b, Iterator e)  {
     typedef SubsetsIterator<Iterator, k, Pointer, Reference> SI;
     return std::make_pair(SI(b,e), SI(e,e));
}

} //data_structures
} //paal

#endif // SUBSET_ITERATOR_HPP

