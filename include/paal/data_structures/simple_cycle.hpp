/**
 * @file simple_cycle.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */

#ifndef SIMPLE_CYCLE_HPP
#define SIMPLE_CYCLE_HPP 

#include <cassert>
#include <map>
#include <vector>
#include <iterator>

#include "paal/helpers/bimap.hpp"

namespace paal {
namespace data_structures {


//TODO THIS IS SIMPLIEST IMPLEMENTATION, LATER WE NEED EFFICIENT IMPLEMENTATION 
//
//TODO MOVE CONSTRUCTORS
template <typename CycleEl, typename IdxT = int> class  SimpleCycle {
    public:
        typedef std::pair<CycleEl, CycleEl> CycleElPair;
        typedef CycleEl CycleElement;

        template <typename Iter> SimpleCycle(Iter begin, Iter end) {
            if(begin == end) {
                return;
            }
            
            size_t size = std::distance(begin, end);

            m_predecessorMap.reserve(size);
            m_successorMap.reserve(size);
            
            IdxT prevIdx = add(*(begin++));
            IdxT firstIdx = prevIdx;
            for(;begin != end;++begin) {
                IdxT lastIdx =  add(*begin);
                link(prevIdx, lastIdx);
                prevIdx = lastIdx;
            }
            link(prevIdx, firstIdx);
        }
        
        //after flip the order will be reversed, ie it will be from 'end'  to 'begin'
        void flip(const CycleEl & begin, const CycleEl & end) {
            IdxT e1 = toIdx(begin);
            IdxT b1 = prevIdx(e1);
            IdxT b2 = toIdx(end);
            IdxT e2 = nextIdx(b2);

            partialReverse(b2, e1);
            link(b1, b2);
            link(e1, e2);
            
        }

        size_t size() const {
            return  m_predecessorMap.size(); 
        }
        
        CycleEl next(const CycleEl & ce) const {
            return fromIdx(nextIdx(toIdx(ce)));
        }
        
        class EdgeIterator : public std::iterator<std::forward_iterator_tag, CycleElPair, 
                                    ptrdiff_t, CycleElPair *, const CycleElPair &> {
            public:
                EdgeIterator(const SimpleCycle & cm, CycleEl ce ) : 
                    m_cycle(&cm), m_idx(m_cycle->toIdx(ce)), m_first(m_idx) {

                    updateCurr();
                }
                
                EdgeIterator() : m_cycle(NULL) ,m_idx(-1) {}

                EdgeIterator & operator++(){
                    m_idx = nextIdx(m_idx);
                    updateCurr();

                    if(m_idx == m_first) {
                        m_idx = -1;    
                    }

                    return *this;
                }
                
                EdgeIterator operator++(int){
                    EdgeIterator i(*this);
                    operator++();
                    return i;
                }

                bool operator!=(EdgeIterator ei) const {
                    return !operator==(ei);
                }               
                
                bool operator==(EdgeIterator ei) const {
                    return m_idx == ei.m_idx;
                }               
                
                const CycleElPair * const operator->() const {
                    return &m_curr;
                }               

                void operator=(EdgeIterator ei) {
                    m_idx = ei.m_idx; 
                    m_first = ei.m_first;
                    m_curr = ei.m_curr;
                    m_cycle = ei.m_cycle;
                }               

                const CycleElPair & operator*() const {
                    return m_curr;
                }
                
            private:
                void updateCurr() {
                    m_curr.first = m_cycle->fromIdx(m_idx);
                    m_curr.second = m_cycle->fromIdx(nextIdx(m_idx));
                }
                
                IdxT nextIdx(IdxT i) const {
                    return m_cycle->nextIdx(i);
                }

                const SimpleCycle * m_cycle;
                IdxT m_idx;
                IdxT m_first;
                CycleElPair m_curr;
        };
        
        typedef std::pair<EdgeIterator, EdgeIterator> EdgeIteratorRange;
        
        EdgeIteratorRange getEdgeRange(const CycleEl & el) const {
            return EdgeIteratorRange(EdgeIterator(*this, el), EdgeIterator());
        }
        
        EdgeIteratorRange getEdgeRange() const {
            return getEdgeRange(fromIdx(0));
        }

    private:
        void link(IdxT x, IdxT y) {
            m_successorMap[x] = y;
            m_predecessorMap[y] = x;
        }

        void partialReverse(IdxT x, IdxT y) {
            if(x == y)
                return;
            IdxT t_next = prevIdx(x);
            IdxT t;
            do {
                t = t_next;
                t_next = prevIdx(t);
                link(x,t);
                x = t;
            } while(t != y);
        }

        IdxT toIdx(const CycleEl & ce) const {
            return m_cycleIdx.getIdx(ce);
        }

        IdxT nextIdx(IdxT i) const {
            return m_successorMap[i];
        }
        
        IdxT prevIdx(IdxT i) const {
            return m_predecessorMap[i];
        }

        const CycleEl & fromIdx(IdxT i) const {
            return m_cycleIdx.getVal(i);
        }

        IdxT add(const CycleEl & el) {
            m_predecessorMap.push_back(-1);
            m_successorMap.push_back(-1);
            return m_cycleIdx.add(el);
        }
        
        helpers::BiMap<CycleEl, IdxT> m_cycleIdx;

        typedef std::vector<IdxT> SorsMap;

        SorsMap m_predecessorMap;
        SorsMap m_successorMap;
};

/*template <typename CycleEl> class  SimpleCycle : 
    public SimpleCycleImpl<CycleEl> {
        public:
//        using SimpleCycleImpl<CycleEl>::SimpleCycleImpl;
        template <typename Iter> SimpleCycle(Iter begin, Iter end) : SimpleCycleImpl<CycleEl>(begin, end) {}
};*/



} //data_structures
} //paal

#endif /* SIMPLE_CYCLE_HPP */
