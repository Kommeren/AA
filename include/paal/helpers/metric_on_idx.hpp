/**
 * @file metric_on_idx.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-14
 */
#ifndef METRIC_ON_IDX_HPP
#define METRIC_ON_IDX_HPP 


#include "bimap.hpp"

namespace paal {
namespace helpers {

template <typename Metric>
class MetricOnIdx {
public:
    typedef typename Metric::DistanceType DistanceType;
    typedef typename Metric::VertexType   VertexType;

    MetricOnIdx(const Metric & m, const helpers::BiMap<VertexType> & idx) : m_metric(m), m_idx(idx) {}

    DistanceType operator()(int i, int j) const {
        return m_metric(m_idx.getVal(i), m_idx.getVal(j));
    }
private:
    const Metric & m_metric;
    const helpers::BiMap<VertexType> & m_idx;
};
}
}
#endif /* METRIC_ON_IDX_HPP */
