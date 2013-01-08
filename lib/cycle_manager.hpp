#include <assert.h>
#include <map>
#include <vector>
#include <iterator>

template <typename CycleEl, typename IdxT = int> class  CycleManager {
    public:
        typedef std::pair<CycleEl, CycleEl> CycleElPair;

        template <typename Iter> CycleManager(Iter begin, Iter end) {
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

        void swapEnds(const CycleElPair & p1, const CycleElPair & p2 ) {
            IdxT b1 = m_mappingToIdx[p1.first];
            IdxT e1 = m_mappingToIdx[p1.second];
            IdxT b2 = m_mappingToIdx[p2.first];
            IdxT e2 = m_mappingToIdx[p2.second];

            partialReverse(b2, e1);
            link(b1, b2);
            link(e1, e2);
            
        }

        size_t size() const {
            return  m_predecessorMap.size(); 
        }

        //TODO not finished
        class EdgeIterator : public std::iterator<std::forward_iterator_tag, CycleElPair> {
            public:
                EdgeIterator(const CycleManager & cm, CycleEl ce ) : 
                    m_cycleManager(&cm), m_idx(m_cycleManager->toIdx(ce)), m_first(m_idx) {

                    updateCurr();
                }
                
                EdgeIterator() : m_cycleManager(NULL) ,m_idx(-1) {}

                void operator++(){
                    m_idx = m_cycleManager->m_successorMap[m_idx];
                    updateCurr();

                    if(m_idx == m_first) {
                        m_idx = -1;    
                    }
                }

                bool operator!=(EdgeIterator ei) const {
                    return m_idx != ei.m_idx;
                }               
                
                void operator=(EdgeIterator ei) {
                    m_idx = ei.m_idx; 
                    m_first = ei.m_first;
                    m_cycleManager = ei.m_cycleManager;
                }               

                const CycleElPair & operator*() const {
                    return m_curr;
                }

            private:
                void updateCurr() {
                    m_curr.first = m_cycleManager->m_mappingFromIdx[m_idx];
                    m_curr.second = m_cycleManager->m_mappingFromIdx[m_cycleManager->m_successorMap[m_idx]];
                }
                const CycleManager * m_cycleManager;
                IdxT m_idx;
                IdxT m_first;
                CycleElPair m_curr;
        };
        
        typedef std::pair<EdgeIterator, EdgeIterator> EdgeIteratorRange;
        
        EdgeIteratorRange getEdgeRange(const CycleEl & el) {
            return EdgeIteratorRange(EdgeIterator(*this, el), EdgeIterator());
        }

    private:
        void link(IdxT x, IdxT y) {
            m_successorMap[x] = y;
            m_predecessorMap[y] = x;
        }

        void partialReverse(IdxT x, IdxT y) {
            IdxT t_next = m_predecessorMap[x];
            IdxT t;
            do {
                t = t_next;
                t_next = m_predecessorMap[t];
                link(x,t);
                x = t;
            } while(t != y);
        }

        IdxT toIdx(const CycleEl & ce) const {
            typename MappingToIdx::const_iterator i = m_mappingToIdx.find(ce);
            assert(i != m_mappingToIdx.end());
            return i->second;
        }

        IdxT add(const CycleEl & el) {
            assert(m_mappingToIdx.find(el) == m_mappingToIdx.end());
            IdxT idx = size();
            m_predecessorMap.push_back(-1);
            m_successorMap.push_back(-1);
            m_mappingFromIdx.push_back(el);
            m_mappingToIdx[el] = idx;
            return idx;
        }
        
        typedef std::vector<IdxT> SorsMap;
        typedef std::map<CycleEl, IdxT> MappingToIdx;
        typedef std::vector<CycleEl> MappingFromIdx;

        MappingToIdx m_mappingToIdx;
        MappingFromIdx m_mappingFromIdx;
        SorsMap m_predecessorMap;
        SorsMap m_successorMap;
};
