/**
 * @file collection_starts_from_last_change.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-07-11
 */
#ifndef COLLECTION_STARTS_FROM_LAST_CHANGE_HPP
#define COLLECTION_STARTS_FROM_LAST_CHANGE_HPP

#include <boost/range/join.hpp>

namespace paal {
namespace data_structures {

    /**
     * @brief this collection stores some range and expose setLastChange function
     *        each time begin and end is called this class returns range which starts from last change place
     *
     * @tparam Iterator
     * @tparam hash
     */
template <typename Iterator, typename hash = std::hash<typename std::iterator_traits<Iterator>::value_type>>
class CollectionStartsFromLastChange {
    typedef typename std::iterator_traits<Iterator>::value_type Element;
    typedef std::unordered_map<Element, Iterator, hash> ElemToIter;
    typedef std::pair<Iterator, Iterator> Range;
    typedef boost::joined_range<Range, Range> JoinedRange;
    typedef typename boost::range_iterator<JoinedRange>::type JoinedIterator;
public:
    typedef JoinedIterator ResultIterator;

    CollectionStartsFromLastChange() = default;

    /**
     * @brief constructor
     *
     * @param begin
     * @param end
     */
    CollectionStartsFromLastChange(Iterator begin, Iterator end) :
        m_begin(begin), m_end(end), m_newBegin(m_begin) {
            assert(m_begin != m_end);
            for(auto i = m_begin; i != m_end; ++i) {
                bool b = m_elemToIter.emplace(*i, i).second;
                assert(b);
            }
        }

    /**
     * @brief one can set the place of the last change (future start position of the range)
     *
     * @param el
     */
    void setLastChange(const Element & el) {
        auto i = m_elemToIter.find(el);
        assert(i != m_elemToIter.end());
        m_newBegin = i->second;
    }

    /**
     * @brief begin
     *
     * @return
     */
    JoinedIterator begin() {
        return boost::begin(getRange());
    }

    /**
     * @brief end
     *
     * @return
     */
    JoinedIterator end()  {
        return boost::end(getRange());
    }

private:
    /**
     * @brief gets range
     *
     * @return
     */
    JoinedRange getRange()  {
        Range r1 = std::make_pair(m_newBegin, m_end);
        Range r2 = std::make_pair(m_begin, m_newBegin);
        return boost::join(r1, r2);
    }

    Iterator m_begin;
    Iterator m_end;
    Iterator m_newBegin;
    ElemToIter m_elemToIter;
};

}
}


#endif /* COLLECTION_STARTS_FROM_LAST_CHANGE_HPP */
