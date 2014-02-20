/**
 * @file 2_local_search_solution_adapter.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */
#ifndef TWO_LOCAL_SEARCH_SOLUTION_ADAPTER_HPP
#define TWO_LOCAL_SEARCH_SOLUTION_ADAPTER_HPP

#include "paal/data_structures/vertex_to_edge_iterator.hpp"

namespace paal {
namespace local_search {
namespace two_local_search {

    /**
     * @brief adapts cycle to have begin and end pointing to edge collection
     *
     * @tparam Cycle
     */
template < typename Cycle> class TwoLocalSearchAdapter  {
    public:
        typedef typename Cycle::VertexIterator VertexIterator;
        typedef data_structures::VertexToEdgeIterator<VertexIterator> Iterator;

        /**
         * @brief constructor
         *
         * @param cm
         */
        TwoLocalSearchAdapter(Cycle & cm) : m_cycle(cm) {}

        /**
         * @brief Edges begin
         *
         * @return
         */
        Iterator begin() const {
            return data_structures::make_VertexToEdgeIterator(m_cycle.vbegin(), m_cycle.vend());
        }

        /**
         * @brief Edges end
         *
         * @return
         */
        Iterator end() const {
            auto end =  m_cycle.vend();
            return data_structures::make_VertexToEdgeIterator(end, end);
        }

        /**
         * @brief gets adopted cycle
         *
         * @return
         */
        Cycle & getCycle() {
            return m_cycle;
        }

        /**
         * @brief gets adopted cycle const version
         *
         * @return
         */
        const Cycle & getCycle() const {
            return m_cycle;
        }

    private:

        Cycle & m_cycle;
};


} //two_local_search
} //local_search
} //paal

#endif // TWO_LOCAL_SEARCH_SOLUTION_ADAPTER_HPP
