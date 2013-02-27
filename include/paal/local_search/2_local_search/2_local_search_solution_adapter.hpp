/**
 * @file 2_local_search_solution_adapter.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */
#ifndef 2_LOCAL_SEARCH_SOLUTION_ADAPTER_HPP
#define 2_LOCAL_SEARCH_SOLUTION_ADAPTER_HPP

#include "paal/helpers/vertex_to_edge_iterator.hpp"

namespace paal {
namespace local_search {
namespace two_local_search {

template < typename Cycle> class TwoLocalSearchAdapter  {
    public:
        typedef typename Cycle::VertexIterator VertexIterator;
        typedef helpers::VertexToEdgeIterator<VertexIterator> Iterator;
        TwoLocalSearchAdapter(Cycle & cm) : m_cycle(cm) {}

        Iterator begin() const {
            return helpers::make_VertexToEdgeIterator(m_cycle.vbegin(), m_cycle.vend());
        }
        
        Iterator end() const {
            auto end =  m_cycle.vend();
            return helpers::make_VertexToEdgeIterator(end, end);
        }

        decltype(std::declval<Cycle>().getCycle()) get()  {
            return m_cycle.getCycle();
        }
        
        Cycle & getWrap() {
            return m_cycle;
        }
    private:

        Cycle & m_cycle;
};


} //two_local_search
} //local_search
} //paal

#endif // 2_LOCAL_SEARCH_SOLUTION_ADAPTER_HPP
