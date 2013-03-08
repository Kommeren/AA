/**
 * @file vertex_to_swap_update.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-03-08
 */
#ifndef VERTEX_TO_SWAP_UPDATE_HPP
#define VERTEX_TO_SWAP_UPDATE_HPP 
#include "facility_location_update_element.hpp"

namespace paal {
namespace local_search {
namespace facility_location {

template <typename VertexType> class VertexToSwapUpdate {
public:
    VertexToSwapUpdate(VertexType v) : m_u(&m_sw), m_from(v) {}

    VertexToSwapUpdate() : m_u(&m_sw) {}
    
    VertexToSwapUpdate(const VertexToSwapUpdate & u) : m_u(&m_sw),  m_from(u.m_from) { 
    }
    
    VertexToSwapUpdate & operator=(const VertexToSwapUpdate & u)  { 
        m_from = u.m_from;
        return *this;
    }

    const Update & operator()(VertexType v) const {
        m_sw.setFrom(m_from); 
        m_sw.setTo(v);
        return m_u;
    }

private:
    mutable Swap<VertexType> m_sw;
    Update m_u;
    VertexType m_from;
};
}
}
}
#endif /* VERTEX_TO_SWAP_UPDATE_HPP */
