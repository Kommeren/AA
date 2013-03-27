/**
 * @file fl_algo.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-15
 */
#ifndef FL_ALGO_HPP
#define FL_ALGO_HPP 

#include "paal/data_structures/metric/metric_traits.hpp"
#include "paal/utils/iterator_utils.hpp"

namespace paal {
namespace simple_algo {

    template <typename Metric, typename FCosts, typename FLSolution> 
       typename data_structures::MetricTraits<Metric>::DistanceType
getFLCost(const Metric & m, const FCosts & fcosts, const FLSolution & fls) {
    auto const & ch      = fls.getChosenFacilities();

    typedef data_structures::MetricTraits<Metric> MT;
    typedef typename MT::DistanceType Dist;
    typedef typename MT::VertexType VertexType;

    Dist d = std::accumulate(ch.begin(), ch.end(), double(0.), [&](Dist d, VertexType f){return d+fcosts(f);});

    for(VertexType f : ch) {
        for(VertexType v : utils::make_range(fls.getClientsForFacility(f))) {
            d += m(v, f); 
        }
    }

    return d;
}

}//paal
}//simple_algo
#endif /* FL_ALGO_HPP */
