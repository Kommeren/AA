#ifndef __2_LOCAL_SEARCH_CONTAINER__
#define __2_LOCAL_SEARCH_CONTAINER__


namespace paal {
namespace local_search {
namespace two_local_search {

template < typename Cycle> class TwoLocalSearchContainer  {
    public:
        typedef typename Cycle::EdgeIterator Iterator;
        TwoLocalSearchContainer(Cycle & cm) : m_cycle(cm) {}

        Iterator begin() const {
            return m_cycle.getEdgeRange().first;
        }
        
        Iterator end() const {
            return m_cycle.getEdgeRange().second;
        }

        Cycle & getCycle() {
            return m_cycle;
        }
    private:

        Cycle & m_cycle;
};


} //two_local_search
} //local_search
} //paal

#endif // __2_LOCAL_SEARCH_CONTAINER__
