/**
 * @file k_median.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-03-08
 */
#ifndef K_MEDIAN_HPP
#define K_MEDIAN_HPP 

#include "paal/local_search/multi_solution_step/local_search_multi_solution.hpp"

#include "paal/local_search/facility_location/facility_location_checker.hpp"
#include "paal/local_search/facility_location/facility_location_updater.hpp"
#include "paal/local_search/facility_location/facility_location_solution_adapter.hpp"


#include "k_median_solution.hpp"
#include "k_median_get_neighbor.hpp"

namespace paal {
namespace local_search {
namespace k_median {

/**
 * @class DefaultKMedianComponents 
 * @brief Model of MultiSearchComponents with default multi search components for k-median.
 *
 * @tparam VertexType
 */
template <typename VertexType> 
struct DefaultKMedianComponents {
    typedef MultiSearchComponents<
                KMedianGetNeighborhood         <VertexType>,
                facility_location::FacilityLocationChecker        <VertexType>,
                facility_location::FacilityLocationUpdater        <VertexType>> type;
};


/**
 * @class KMedianLocalSearchStep
 * @brief this is model of LocalSearchStepMultiSolution concept. See \ref local_search.<br>
 * KMedian implementation is based on facility location solution. <br>
 * The Update is facility_location::Update. <br>
 * The Solution is adapted KMedianSolution. <br>
 * The SolutionElement is facility_location::Facility  <br>
 * Use DefaultKMedianComponents for default search components.
 *
 * The KMedianLocalSearchStep takes as constructor parameter  KMedianSolution.
 * <b> WARNING </b>
 * getSolution of the KMedianLocalSearchStep returns type ObjectWithCopy<KMedianSolution>.
 * If you want to perform search, then change the solution object and continue local search you should perform all the operations on ObjectWithCopy. <br>
 * example: 
    \snippet k_median_example.cpp K Median Search Example
 *
 * full example is k_median_example.cpp
 *
 * @tparam Voronoi
 * @tparam MultiSearchComponents
 */
template <int k, typename Voronoi,
          typename MultiSearchComponents = typename DefaultKMedianComponents<typename Voronoi::VertexType>::type>

class KMedianLocalSearchStep : 
    public LocalSearchStepMultiSolution<
               facility_location::FacilityLocationSolutionAdapter<
                    KMedianSolution<k, Voronoi>>,
               search_strategies::ChooseFirstBetter,
               MultiSearchComponents>  {

public:
    typedef KMedianSolution<k, Voronoi> KMSolution;
    typedef facility_location::FacilityLocationSolutionAdapter<KMSolution> KMSolutionAdapter;
    
    typedef LocalSearchStepMultiSolution<
                KMSolutionAdapter,
                search_strategies::ChooseFirstBetter,
                MultiSearchComponents>  base;

    KMedianLocalSearchStep(
            KMSolution kms,
            MultiSearchComponents sc = MultiSearchComponents()) :
                base(KMSolutionAdapter(std::move(kms)), 
                                       std::move(sc)) {}
};



}
}
}

#endif /* K_MEDIAN_HPP */
