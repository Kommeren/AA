/**
 * @file k_median_solution.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-03-08
 */
#ifndef K_MEDIAN_SOLUTION_HPP
#define K_MEDIAN_SOLUTION_HPP 

#include "paal/utils/do_notihng_functor.hpp"
#include "paal/data_structures/facility_location/facility_location_solution.hpp"

namespace paal {
namespace local_search {
namespace k_median {

template <int k, typename VoronoiType>
class KMedianSolution : 
    public data_structures::FacilityLocationSolution<utils::ReturnZeroFunctor, VoronoiType> {
    typedef data_structures::FacilityLocationSolution<utils::ReturnZeroFunctor, VoronoiType> base;
public:
    KMedianSolution(VoronoiType voronoi,
                    typename base::UnchosenFacilitiesSet uf) : 
        base(std::move(voronoi), std::move(uf), m_zeroFunc) {assert(base::getChosenFacilities() == k);}
private:
    utils::ReturnZeroFunctor m_zeroFunc;
        
};

}
}
}


#endif /* K_MEDIAN_SOLUTION_HPP */
