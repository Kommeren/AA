/**
 * @file bimap.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-14
 */
#ifndef BIMAP_HPP
#define BIMAP_HPP 

/*#if !defined(NDEBUG)
#define BOOST_MULTI_INDEX_ENABLE_INVARIANT_CHECKING
//#define BOOST_MULTI_INDEX_ENABLE_SAFE_MODE
#endif

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <iostream>
#include <string>*/

#include "paal/helpers/iterator_helpers.hpp"

namespace paal {
namespace data_structures {


//minor TODO write specification when T is integral (copy instead of reference)
template <typename T, typename Idx = int> class BiMap {
public:
    
    BiMap() {}

    template <typename Iter> BiMap(Iter b, Iter e) {
        for(const T & t : helpers::make_range(b,e)) {
            add(t);
        }
    }
    
    BiMap(BiMap && bm) : 
        m_idToT(std::move(bm.m_idToT)), 
        m_tToID(std::move(bm.m_tToID)) {
    }
    
    BiMap(const BiMap & bm) : 
        m_idToT(bm.m_idToT), 
        m_tToID(bm.m_tToID) {
    }

    Idx getIdx(const T & t) const {
        auto iter = m_tToID.find(t);
        assert(iter != m_tToID.end());
        return iter->second;
    }

    const T & getVal(Idx i) const {
        return m_idToT[i];
    }
    
    size_t size() const {
        return  m_idToT.size(); 
    }

    Idx add(const T & t) {
        assert(m_tToID.find(t) == m_tToID.end());
        Idx idx = size();
        m_tToID[t] = idx;
        m_idToT.push_back(t);
        return idx;
    }
private:
    std::vector<T> m_idToT;
    std::map<T,Idx> m_tToID;
};

template <typename T, typename Idx = int> class BiMapOfConsecutive {
    //TODO mayby it should be passed but only on debug
    static const Idx INVALID_IDX = -1;
public:
    static_assert(std::is_integral<T>::value, "Type T has to be integral");
    BiMapOfConsecutive() {}

    template <typename Iter> BiMapOfConsecutive(Iter b, Iter e) {
        if(b == e)
            return;
        size_t size = *max_element(b, e);
        m_idToT.resize(size);
        m_tToID.resize(size, INVALID_IDX);
        std::copy(b, e, m_idToT.begin());
        for(size_t i = 0; i < size; ++i) {
            Idx & idx = m_tToID[m_idToT[i]]; 
            assert(m_idToT[i] < size && idx == INVALID_IDX);
            idx = i;
        }
    }

    Idx getIdx(const T & t) const {
        return m_tToID[t];
    }

    const T & getVal(Idx i) const {
        return m_idToT[i];
    }
    
    size_t size() const {
        return  m_idToT.size(); 
    }

private:
    std::vector<T>   m_idToT;
    std::vector<Idx> m_tToID;
};

} //data_structures
} //paal
#endif /* BIMAP_HPP */
