#ifndef __2_LOCAL_SEARCH_CONTAINER__
#define __2_LOCAL_SEARCH_CONTAINER__


namespace paal {
namespace local_search {
namespace two_local_search {

template < typename CycleManager> class TwoLocalSearchContainer  {
    public:
        typedef typename CycleManager::EdgeIterator Iterator;
        TwoLocalSearchContainer(CycleManager & cm) : m_cycleManager(cm) {}

        Iterator begin() const {
            return m_cycleManager.getEdgesRange().first;
        }
        
        Iterator end() const {
            return m_cycleManager.getEdgesRange().second;
        }

        CycleManager & m_cycleManager;
};


} //two_local_search
} //local_search
} //paal

#endif // __2_LOCAL_SEARCH_CONTAINER__
