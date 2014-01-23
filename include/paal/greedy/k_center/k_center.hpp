/**
 * @file k_center.hpp
 * @brief
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2014-01-23
 */
#ifndef K_CENTER
#define K_CENTER

#include "paal/data_structures/metric/metric_traits.hpp"

namespace paal{
namespace greedy{
/**
 * @brief this is solve K Center problem
 * and return radius
 * example:
 *  \snippet k_center_example.cpp K Center Example
 *
 * complete example is k_center_example.cpp
 * @param Metric metric
 * @param int numberOfClusters
 * @param OutputIterator result ItemIterators
 * @tparam ArrayMetric
 * @tparam OutputIterator
 * @tparam ItemIterator
 */
template<typename Metric,class OutputIterator,typename ItemIterator>
auto kCenter(const Metric& metric,unsigned int numberOfClusters, const ItemIterator iBegin, const ItemIterator iEnd, OutputIterator result) ->
        typename data_structures::MetricTraits<Metric>::DistanceType{

        typedef typename data_structures::MetricTraits<Metric>::DistanceType Dist;
        std::vector<Dist> distance_from_closest_center(std::distance(iBegin,iEnd),std::numeric_limits<Dist>::max());
        ItemIterator last_centre=iBegin;
        ItemIterator farthest_centre=iBegin;
        Dist radious;
        ++numberOfClusters;
        while(--numberOfClusters){
            *result= *farthest_centre;
            ++result;
            radious=std::numeric_limits<Dist>::min();
            auto it=distance_from_closest_center.begin();
            for(auto i=iBegin;i!=iEnd;++i){
                *it=std::min(*it,metric(*last_centre,*i));
                if(*it>radious){
                    farthest_centre=i;
                    radious=*it;
                }
                ++it;
            }
            last_centre=farthest_centre;
        }
        return radious;
}

}//!greedy
}//!paal

#endif /* K_CENTER */