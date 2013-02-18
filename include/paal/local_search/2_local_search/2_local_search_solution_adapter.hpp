/**
 * @file 2_local_search_solution_adapter.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */
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

        Cycle & get() {
            return m_cycle;
        }
    private:

        Cycle & m_cycle;
};


} //two_local_search
} //local_search
} //paal

#endif // __2_LOCAL_SEARCH_CONTAINER__
