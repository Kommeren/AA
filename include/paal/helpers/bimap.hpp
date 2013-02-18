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
namespace helpers {

/*namespace bmi = boost::multi_index;

// tags for accessing both sides of a bidirectional map 

struct from{};
struct to{};

// The class template bidirectional_map wraps the specification
// of a bidirectional map based on multi_index_container.
//

template<typename FromType,typename ToType>
struct bidirectional_map {

    struct value_type {
        value_type(const FromType& first_,const ToType& second_):
            first(first_),second(second_)
        {}

        FromType first;
        ToType   second;
    };


    // A bidirectional map can be simulated as a multi_index_container
    // of pairs of (FromType,ToType) with two unique indices, one
    // for each member of the pair.
    //

    typedef bmi::multi_index_container<
        value_type,
        bmi::indexed_by<
            bmi::ordered_unique<
            bmi::tag<from>, bmi::member<value_type,FromType, &value_type::first > >,
        bmi::ordered_unique<
            bmi::tag<to>,   bmi::member<value_type,ToType,   &value_type::second> > >
                       > type;

};*/


//minor TODO write specification when T is integral (copy instead of reference)
template <typename T, typename Idx = int> class BiMap {
public:
    
    BiMap() {}

    template <typename Iter> BiMap(Iter b, Iter e) {
        for(const T & t : make_range(b,e)) {
            add(t);
        }
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

}
}
#endif /* BIMAP_HPP */
