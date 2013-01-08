#include <assert.h>
#include <map>
#include <vector>

template <typename CycleEl> class  CycleManager {
    typedef int idx_t;
    public:
        typedef std::pair<CycleEl, CycleEl> CycleElPair;

        template <typename Iter> CycleManager(Iter begin, Iter end) {
            if(begin == end) {
                return;
            }
            
            size_t size = std::distance(begin, end);

            m_predecessorMap.reserve(size);
            m_successorMap.reserve(size);
            
            idx_t prevIdx = add(*(begin++));
            idx_t firstIdx = prevIdx;
            for(;begin != end;++begin) {
                idx_t lastIdx =  add(*begin);
                link(prevIdx, lastIdx);
                prevIdx = lastIdx;
            }
            link(prevIdx, firstIdx);
        }

        void swapEnds(const CycleElPair & p1, const CycleElPair & p2 ) {
            idx_t b1 = m_mappingToIdx[p1.first];
            idx_t e1 = m_mappingToIdx[p1.second];
            idx_t b2 = m_mappingToIdx[p2.first];
            idx_t e2 = m_mappingToIdx[p2.second];

            partialReverse(b2, e1);
            link(b1, b2);
            link(e1, e2);
            
        }

        size_t size() const {
            return  m_predecessorMap.size(); 
        }

        //TODO not finished
        class EdgeIterator {
            public:
                EdgeIterator(CycleEl ce) : m_idx(m_mappingToIdx[ce]) {
                    updateCurr();
                }

                void operator++(){
                    m_idx = m_successorMap[m_idx];
                    updateCurr;
                }

                bool operator!=(EdgeIterator ei) const {
                    return m_idx != ei.m_idx;
                }                

                const CycleElPair & operator*() const {
                    return m_curr;
                }

            private:
                void updateCurr() {
                    m_curr.first = m_mappingFromIdx[m_idx];
                    m_curr.second = m_mappingFromIdx[m_successorMap[m_idx]];
                }
                idx_t m_idx;
                CycleElPair m_curr;
        };
        
        typedef std::pair<EdgeIterator, EdgeIterator> EdgeIteratorRange;
        
        EdgeIteratorRange getEdgeRange() {
            return EdgeIteratorRange(EdgeIterator());
        }

    private:
        void link(idx_t x, idx_t y) {
            m_successorMap[x] = y;
            m_predecessorMap[y] = x;
        }

        void partialReverse(idx_t x, idx_t y) {
            idx_t t_next = m_predecessorMap[x];
            idx_t t;
            do {
                t = t_next;
                t_next = m_predecessorMap[t];
                link(x,t);
                x = t;
            } while(t != y);
        }

        idx_t add(const CycleEl & el) {
            assert(m_mappingToIdx.find(el) == m_mappingToIdx.end());
            idx_t idx = size();
            m_predecessorMap.push_back(-1);
            m_successorMap.push_back(-1);
            m_mappingFromIdx.push_back(el);
            m_mappingToIdx[el] = idx;
            return idx;
        }

        std::map<CycleEl, idx_t> m_mappingToIdx;
        std::vector<CycleEl> m_mappingFromIdx;
        typedef std::vector<idx_t> SorsMap;
        SorsMap m_predecessorMap;
        SorsMap m_successorMap;
};
