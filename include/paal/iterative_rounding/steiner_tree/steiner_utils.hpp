/**
 * @file steiner_utils.hpp
 * @brief
 * @author Maciej Andrejczuk
 * @version 1.0
 * @date 2013-08-01
 */
#ifndef STEINER_UTILS_HPP
#define STEINER_UTILS_HPP

#include <boost/graph/prim_minimum_spanning_tree.hpp>
#include <boost/range/join.hpp>

#include "paal/data_structures/bimap.hpp"
#include "paal/data_structures/metric/metric_to_bgl.hpp"

namespace paal {
namespace ir {

class SteinerUtils {
public:
    /**
     * Calculates a total cost of given edges.
     */
    template<typename Metric, typename Terminals, typename Result>
    static typename data_structures::MetricTraits<Metric>::DistanceType
    countCost(const Result& steinerVertices, const Terminals& terminals, const Metric& costMap) {

        typedef typename data_structures::MetricTraits<Metric>::VertexType Vertex;
        typedef typename data_structures::MetricTraits<Metric>::DistanceType Dist;
        auto allElements = boost::range::join(terminals, steinerVertices);
        paal::data_structures::BiMap<Vertex> idx;
        auto g = paal::data_structures::metricToBGLWithIndex(costMap,
                boost::begin(allElements), boost::end(allElements), idx);
        std::vector<Vertex> pm(allElements.size());
        boost::prim_minimum_spanning_tree(g, &pm[0]);
        auto idxM = paal::data_structures::make_metricOnIdx(costMap, idx);
        Dist cost = 0;
        for(int i : boost::irange(0, int(pm.size()))) {
            cost += idxM(i, pm[i]);
        }
        return cost;
    }
};

} //ir
} //paal

#endif /* STEINER_UTILS_HPP */
