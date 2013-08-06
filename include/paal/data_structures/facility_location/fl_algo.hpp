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
#include "paal/utils/do_nothing_functor.hpp"

namespace paal {
namespace simple_algo {

    template <typename Metric, typename FCosts, typename FLSolution> 
       typename data_structures::MetricTraits<Metric>::DistanceType
getCFLCost(const Metric & m, const FCosts & fcosts, const FLSolution & fls) {
    auto const & ch      = fls.getChosenFacilities();

    typedef data_structures::MetricTraits<Metric> MT;
    typedef typename MT::DistanceType Dist;
    typedef typename MT::VertexType VertexType;

    Dist d = std::accumulate(ch.begin(), ch.end(), Dist(0), [&](Dist d, VertexType f){return d+fcosts(f);});

    for(VertexType f : ch) {
        for(std::pair<VertexType, Dist> v : utils::make_range(fls.getClientsForFacility(f))) {
            d += m(v.first, f) * v.second; 
        }
    }

    return d;
}
    
template <typename Metric, typename FCosts, typename FLSolution> 
       typename data_structures::MetricTraits<Metric>::DistanceType
getFLCost(const Metric & m, const FCosts & fcosts, const FLSolution & fls) {
    auto const & ch      = fls.getChosenFacilities();

    typedef data_structures::MetricTraits<Metric> MT;
    typedef typename MT::DistanceType Dist;
    typedef typename MT::VertexType VertexType;

    Dist d = std::accumulate(ch.begin(), ch.end(), Dist(0), [&](Dist d, VertexType f){return d+fcosts(f);});

    for(VertexType f : ch) {
        for(VertexType v : utils::make_range(fls.getClientsForFacility(f))) {
            d += m(v, f); 
        }
    }

    return d;
}

template <typename Metric, typename FLSolution>
        typename data_structures::MetricTraits<Metric>::DistanceType
getKMCost(const Metric & m, const FLSolution & fls){
    utils::ReturnZeroFunctor m_zeroFunc;
    return paal::simple_algo::getFLCost(std::move(m), m_zeroFunc, std::move(fls));
}

}//simple_algo
}//paal
#endif /* FL_ALGO_HPP */
