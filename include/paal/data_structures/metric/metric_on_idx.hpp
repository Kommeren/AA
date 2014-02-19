/**
 * @file metric_on_idx.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-14
 */
#ifndef METRIC_ON_IDX_HPP
#define METRIC_ON_IDX_HPP


#include "paal/data_structures/bimap_traits.hpp"

namespace paal {
namespace data_structures {

template <typename Metric, typename Bimap>
class MetricOnIdx {
public:
    typedef data_structures::MetricTraits<Metric> MT;
    typedef typename MT::DistanceType DistanceType;
    typedef typename BiMapTraits<Bimap>::Idx  VertexType;

    MetricOnIdx(const Metric & m, const Bimap & idx) : m_metric(m), m_idx(idx) {}

    DistanceType operator()(VertexType i, VertexType j) const {
        return m_metric(m_idx.getVal(i), m_idx.getVal(j));
    }
private:
    const Metric & m_metric;
    const Bimap & m_idx;
};

template <typename Metric, typename Bimap>
MetricOnIdx<Metric, Bimap>
make_metricOnIdx(const Metric & m, const Bimap & b) {
    return MetricOnIdx<Metric, Bimap>(m, b);
}

}
}
#endif /* METRIC_ON_IDX_HPP */
