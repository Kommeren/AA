/**
 * @file euclidean_metric.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-10-28
 */
#ifndef EUCLIDEAN_METRIC_HPP
#define EUCLIDEAN_METRIC_HPP 

#include <cmath>

namespace paal {

struct EuclideanMetric {
    int operator()(const std::pair<int, int> & p1, const std::pair<int, int> & p2) const {
        return sqrt(pow(p1.first - p2.first, 2) + pow(p1.second - p2.second, 2));

    }
};

namespace data_structures { 

    template <>
    struct MetricTraits<EuclideanMetric> {
        typedef std::pair<int, int> VertexType;
        typedef int DistanceType;
    };
}//data_structures

}//paal

#endif /* EUCLIDEAN_METRIC_HPP */
