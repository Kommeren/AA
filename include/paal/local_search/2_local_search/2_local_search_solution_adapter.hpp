/**
 * @file 2_local_search_solution_adapter.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */
#ifndef __2_LOCAL_SEARCH_CONTAINER__
#define __2_LOCAL_SEARCH_CONTAINER__

#include "paal/helpers/vertex_to_edge_iterator.hpp"

namespace paal {
namespace local_search {
namespace two_local_search {

template < typename Cycle> class TwoLocalSearchContainer  {
    public:
        typedef typename Cycle::VertexIterator VertexIterator;
        typedef helpers::VertexToEdgeIterator<VertexIterator> Iterator;
        TwoLocalSearchContainer(Cycle & cm) : m_cycle(cm) {}

        Iterator begin() const {
            return helpers::make_VertexToEdgeIterator(m_cycle.getVerticesRange());
        }
        
        Iterator end() const {
            auto end =  m_cycle.getVerticesRange().second;
            return helpers::make_VertexToEdgeIterator(end, end);
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
