#ifndef __2_LOCAL_SEARCH_CONTAINER__
#define __2_LOCAL_SEARCH_CONTAINER__


namespace paal {
namespace local_search {
namespace two_local_search {

template < typename CycleManager> class TwoLocalSearchContainer  {
    public:
        typedef typename CycleManager::EdgeIterator Iterator;
        TwoLocalSearchContainer(CycleManager & cm) : m_cycleManager(cm) {}

        Iterator cbegin() const {
            return m_cycleManager.getEdgeRange().first;
        }
        
        Iterator cend() const {
            return m_cycleManager.getEdgeRange().second;
        }

        const CycleManager & m_cycleManager;
};


} //two_local_search
} //local_search
} //paal

#endif // __2_LOCAL_SEARCH_CONTAINER__
