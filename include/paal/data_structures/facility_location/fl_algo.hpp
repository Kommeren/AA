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
#include "paal/utils/functors.hpp"

namespace paal {
namespace simple_algo {

/**
 * @brief returns cost for capacitated facility location
 *
 * @tparam Metric
 * @tparam FCosts
 * @tparam FLSolution
 * @param m
 * @param fcosts
 * @param fls
 *
 * @return
 */
template <typename Metric, typename FCosts, typename FLSolution>
typename data_structures::metric_traits<Metric>::DistanceType
get_cfl_cost(const Metric &m, const FCosts &fcosts, const FLSolution &fls) {
    auto const &ch = fls.get_chosen_facilities();

    typedef data_structures::metric_traits<Metric> MT;
    typedef typename MT::DistanceType Dist;
    typedef typename MT::VertexType VertexType;

    Dist d = std::accumulate(ch.begin(), ch.end(), Dist(0),
                             [&](Dist d, VertexType f) {
        return d + fcosts(f);
    });

    for (VertexType f : ch) {
        for (std::pair<VertexType, Dist> v :
             boost::make_iterator_range(fls.get_clients_for_facility(f))) {
            d += m(v.first, f) * v.second;
        }
    }

    return d;
}

/**
 * @brief return cost for facility location
 *
 * @tparam Metric
 * @tparam FCosts
 * @tparam FLSolution
 * @param m
 * @param fcosts
 * @param fls
 *
 * @return
 */
template <typename Metric, typename FCosts, typename FLSolution>
typename data_structures::metric_traits<Metric>::DistanceType
get_fl_cost(const Metric &m, const FCosts &fcosts, const FLSolution &fls) {
    auto const &ch = fls.get_chosen_facilities();

    typedef data_structures::metric_traits<Metric> MT;
    typedef typename MT::DistanceType Dist;
    typedef typename MT::VertexType VertexType;

    Dist d = std::accumulate(ch.begin(), ch.end(), Dist(0),
                             [&](Dist d, VertexType f) {
        return d + fcosts(f);
    });

    for (VertexType f : ch) {
        for (VertexType v :
             boost::make_iterator_range(fls.get_clients_for_facility(f))) {
            d += m(v, f);
        }
    }

    return d;
}

/**
 * @brief returns cost for k-median
 *
 * @tparam Metric
 * @tparam FLSolution
 * @param m
 * @param fls
 *
 * @return
 */
template <typename Metric, typename FLSolution>
typename data_structures::metric_traits<Metric>::DistanceType
get_km_cost(const Metric &m, const FLSolution &fls) {
    utils::return_zero_functor m_zero_func;
    return paal::simple_algo::get_fl_cost(std::move(m), m_zero_func,
                                          std::move(fls));
}

} //! simple_algo
} //! paal
#endif /* FL_ALGO_HPP */
