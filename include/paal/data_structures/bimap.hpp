/**
 * @file bimap.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-14
 */
#ifndef BIMAP_HPP
#define BIMAP_HPP 

#include <map>
#include <unordered_map>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/multi_index/hashed_index.hpp>

#include "paal/utils/iterator_utils.hpp"


namespace paal {
namespace data_structures {

/**
 * @class BiMapMIC
 * @brief the same as Bimap, but implemented using boost::multi_index_container. Unfortunately slover
 *
 * @tparam T
 * @tparam Idx
 */
template <typename T, typename Idx = int> 
class BiMapMIC {
public:
    
    BiMapMIC() {}

    template <typename Iter> BiMapMIC(Iter b, Iter e) {
        size_t s = std::distance(b, e);
        m_index.reserve(s);
        for(const T & t : utils::make_range(b,e)) {
            add(t);
        }
    }
    
    BiMapMIC(BiMapMIC && bm) = default; 
    
    BiMapMIC(const BiMapMIC & bm) =  default;
    
    BiMapMIC & operator=(BiMapMIC && bm)  = default;

    Idx getIdx(const T & t) const {
        auto const & idx = m_index.template get<1>();
        return m_index.template project<0>(idx.find(t)) - m_index.begin();
    }

    const T & getVal(Idx i) const {
#ifdef NDEBUG
        return m_index[i];
#else
        return m_index.at(i);
#endif
    }
    
    size_t size() const {
        return m_index.size();
    }

    Idx add(const T & t) {
        m_index.push_back(t);
        return m_index.size() - 1;
    }
private:
    typedef boost::multi_index_container<T, 
                boost::multi_index::indexed_by<
                    boost::multi_index::random_access<>,
                    boost::multi_index::hashed_unique<boost::multi_index::identity<T>>
                >
            > bm_type;
    bm_type m_index;
};


//minor TODO write specification when T is integral (copy instead of reference)
/**
 * @class BiMapOld
 * @brief implements both sides mapping from the collection to (0,size(collection)) interval.
 *
 * @tparam T
 * @tparam Idx
 */
template <typename T, typename Idx = int> 
class BiMap {
public:
    
    BiMap() {}

    template <typename Iter> BiMap(Iter b, Iter e) {
        size_t s = std::distance(b, e);
        m_idToT.reserve(s);
        m_tToID.reserve(s);
        for(const T & t : utils::make_range(b,e)) {
            add(t);
        }
    }
    
    BiMap(BiMap && bm) = default; 
    
    BiMap(const BiMap & bm) =  default;
    
    BiMap & operator=(BiMap && bm)  = default;

    Idx getIdx(const T & t) const {
        auto iter = m_tToID.find(t);
        assert(iter != m_tToID.end());
        return iter->second;
    }

    const T & getVal(Idx i) const {
#ifdef NDEBUG
        return m_idToT[i];
#else
        return m_idToT.at(i);
#endif
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
    std::unordered_map<T,Idx> m_tToID;
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
