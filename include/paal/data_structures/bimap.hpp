/**
 * @file bimap.hpp
 * @brief
 * @author Piotr Wygocki, Piotr Smulewicz
 * @version 1.1
 * @date 2013-09-12
 */
#ifndef BIMAP_HPP
#define BIMAP_HPP

#include <unordered_map>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/range/irange.hpp>
#include <boost/functional/hash.hpp>

#include "paal/data_structures/bimap_traits.hpp"

namespace paal {
namespace data_structures {

/**
 * @class BiMapMIC
 * @brief the same as Bimap, but implemented using boost::multi_index_container. Unfortunately slower
 *
 * @tparam T
 * @tparam Idx
 */
template <typename T, typename Idx = int>
class BiMapMIC {
public:

    BiMapMIC() = default;

    /**
     * @brief constructor
     *
     * @tparam Iter
     * @param b
     * @param e
     */
    template <typename Iter> BiMapMIC(Iter b, Iter e) {
        std::size_t s = std::distance(b, e);
        m_index.reserve(s);
        for(const T & t : boost::make_iterator_range(b,e)) {
            add(t);
        }
    }


    /**
     * @brief getIdx on element t
     *
     * @param t
     *
     * @return
     */
    Idx getIdx(const T & t) const {
        auto const & idx = m_index.template get<1>();
        return m_index.template project<0>(idx.find(t)) - m_index.begin();
    }

    /**
     * @brief get element on index i
     *
     * @param i
     *
     * @return
     */
    const T & getVal(Idx i) const {
#ifdef NDEBUG
        return m_index[i];
#else
        return m_index.at(i);
#endif
    }

    /**
     * @brief number of elements
     *
     * @return
     */
    std::size_t size() const {
        return m_index.size();
    }

    /**
     * @brief adds alement to bimap
     *
     * @param t
     *
     * @return
     */
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
 * @class BiMap
 * @brief implements both sides mapping from the collection to (0,size(collection)) interval.
 *
 * @tparam T
 * @tparam Idx
 */
template <typename T, typename Idx = int>
class BiMap {
    typedef std::unordered_map<T,Idx, boost::hash<T>> TToID;
public:
    typedef typename TToID::const_iterator Iterator;

    BiMap() = default;

    /**
     * @brief constructor
     *
     * @tparam Iter
     * @param b
     * @param e
     */
    template <typename Iter> BiMap(Iter b, Iter e) {
        std::size_t s = std::distance(b, e);
        m_idToT.reserve(s);
        m_tToID.reserve(s);
        for(const T & t : boost::make_iterator_range(b,e)) {
            add(t);
        }
    }

    /**
     * @brief gets index of element t
     *
     * @param t
     *
     * @return
     */
    Idx getIdx(const T & t) const {
        auto iter = m_tToID.find(t);
        assert(iter != m_tToID.end());
        return iter->second;
    }

    /**
     * @brief get value for index i
     *
     * @param i
     *
     * @return
     */
    const T & getVal(Idx i) const {
#ifdef NDEBUG
        return m_idToT[i];
#else
        return m_idToT.at(i);
#endif
    }

    /**
     * @brief number of elements
     *
     * @return
     */
    std::size_t size() const {
        return  m_idToT.size();
    }

    /**
     * @brief adds element to collection
     *
     * @param t
     *
     * @return
     */
    Idx add(const T & t) {
        assert(m_tToID.find(t) == m_tToID.end());
        Idx idx = size();
        m_tToID[t] = idx;
        m_idToT.push_back(t);
        return idx;
    }

    /**
     * @brief get range of all element, index pairs
     *
     * @return
     */
    std::pair<Iterator, Iterator> getRange() const {
        return std::make_pair(m_tToID.begin(), m_tToID.end());
    }


protected:
    ///mapping from id to element
    std::vector<T> m_idToT;
    ///mapping from elements to ids
    TToID m_tToID;
};


/**
 * @brief this maps support erasing elements, Alert inefficient!!
 *
 * @tparam T
 * @tparam Idx
 */
template <typename T, typename Idx = int>
class EraseableBiMap : public BiMap<T, Idx> {
    typedef BiMap<T, Idx> base;
    using base::m_tToID;
    using base::m_idToT;
public:
    /**
     * @brief erases element (takes linear time)
     *
     * @param t
     */
    void erase(const T & t) {
        auto i = m_tToID.find(t);
        assert(i != m_tToID.end());
        Idx idx = i->second;
        m_tToID.erase(i);
        m_idToT.erase(m_idToT.begin() + idx);

        for(int i : boost::irange(idx, Idx(m_idToT.size()))) {
            assert(m_tToID.at(m_idToT[i]) == i + 1);
            m_tToID[m_idToT[i]] = i;
        }
    }
};

/**
 * @brief in this bimap we know that elements forms permutation
 *        this allows optimization
 *
 * @tparam T
 * @tparam Idx
 */
template <typename T, typename Idx = int> class BiMapOfConsecutive {
    //TODO maybe it should be passed but only on debug
    static const Idx INVALID_IDX = -1;
public:
    static_assert(std::is_integral<T>::value, "Type T has to be integral");
    BiMapOfConsecutive() = default;

    /**
     * @brief constructor
     *
     * @tparam Iter
     * @param b
     * @param e
     */
    template <typename Iter> BiMapOfConsecutive(Iter b, Iter e) {
        if(b == e)
            return;

        std::size_t size = std::distance(b, e);
        m_idToT.resize(size);
        std::copy(b, e, m_idToT.begin());

        m_tToID.resize(size, INVALID_IDX);
        rank(m_idToT,m_tToID,INVALID_IDX);

    }

    /**
     * @brief gets index of element t
     *
     * @param t
     *
     * @return
     */
    Idx getIdx(const T & t) const {
        return m_tToID[t];
    }

    /**
     * @brief gets value for index i
     *
     * @param i
     *
     * @return
     */
    const T & getVal(Idx i) const {
        return m_idToT[i];
    }

    /**
     * @brief number of elements
     *
     * @return
     */
    std::size_t size() const {
        return  m_idToT.size();
    }

private:
    std::vector<T>   m_idToT;
    std::vector<Idx> m_tToID;
};

/**
 * @brief traits specialization for Bimap
 *
 * @tparam ValT
 * @tparam IdxT
 */
template <typename ValT, typename IdxT>
struct BiMapTraits<BiMap<ValT, IdxT>> {
    typedef ValT Val;
    typedef IdxT Idx;
};

/**
 * @brief traits specialization for EraseableBiMap
 *
 * @tparam ValT
 * @tparam IdxT
 */
template <typename ValT, typename IdxT>
struct BiMapTraits<EraseableBiMap<ValT, IdxT>>  {
    typedef ValT Val;
    typedef IdxT Idx;
};

/**
 * @brief traits specialization for BiMapOfConsecutive
 *
 * @tparam ValT
 * @tparam IdxT
 */
template <typename ValT, typename IdxT>
struct BiMapTraits<BiMapOfConsecutive<ValT, IdxT>>  {
    typedef ValT Val;
    typedef IdxT Idx;
};

/**
 * @brief traits specialization for BiMapMIC
 *
 * @tparam ValT
 * @tparam IdxT
 */
template <typename ValT, typename IdxT>
struct BiMapTraits<BiMapMIC<ValT, IdxT>>  {
    typedef ValT Val;
    typedef IdxT Idx;
};

/**
 * @brief computes rank i.e. index of element in range
 *
 * @tparam T
 * @tparam Idx
 * @param m_idToT
 * @param m_tToID
 * @param INVALID_IDX
 */
template <typename T, typename Idx = int>
void rank(std::vector<T> const& m_idToT,std::vector<Idx> &m_tToID,int INVALID_IDX=0){
    static_assert(std::is_integral<T>::value, "Type T has to be integral");
    unsigned long size = m_tToID.size();
    for(auto i : boost::irange(0ul,size)) {
            Idx & idx = m_tToID[m_idToT[i]];
            assert(m_idToT[i] < int(size) && idx == INVALID_IDX);
            idx = i;
            }
}

} //data_structures
} //paal
#endif /* BIMAP_HPP */
