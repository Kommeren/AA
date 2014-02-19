/**
 * @file splay_cycle.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-03-08
 */
#ifndef SPLAY_CYCLE_HPP
#define SPLAY_CYCLE_HPP

#include "paal/data_structures/splay_tree.hpp"
#include "paal/data_structures/bimap.hpp"
#include "paal/data_structures/cycle_iterator.hpp"

namespace paal {
namespace data_structures {


template <typename T>
class SplayCycle {
    typedef splay_tree::template Iterator<T> SIter;
    typedef splay_tree::SplayTree<T> SplayTree;
public:
    typedef CycleIterator<SIter> VIter;
    SplayCycle() {}
    template <typename Iter>
        SplayCycle(Iter begin, Iter end) :
            m_splayTree(begin, end),
            m_size(m_splayTree.size()) {}

    VIter vbegin() const {
        return VIter(m_splayTree.begin(), m_splayTree.begin(), m_splayTree.end());
    }

    VIter vbegin(const T& t) const {
        std::size_t i = m_splayTree.getIdx(t);
        assert(i != std::size_t(-1));
        return VIter(SIter(m_splayTree.splay(i), &m_splayTree), m_splayTree.begin(), m_splayTree.end());
    }

    VIter vend() const {
        auto e = m_splayTree.end();
        return VIter(e, e ,e);
    }

    void flip(const T & begin, const T & end) {
        if(begin == end) {
            return;
        }
        std::size_t b = m_splayTree.getIdx(begin);
        assert(b != std::size_t(-1));
        std::size_t e = m_splayTree.getIdx(end);
        assert(e != std::size_t(-1));
        if(b < e) {
            m_splayTree.reverse(b, e);
        } else {
            m_splayTree.reverse(e + 1, b - 1);
            m_splayTree.reverse(0, m_size-1);
        }
    }

private:
    SplayTree m_splayTree;
    const std::size_t m_size;
};

} //data_structures
} //paal
#endif /* SPLAY_CYCLE_HPP */
