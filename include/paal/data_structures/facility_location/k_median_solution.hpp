/**
 * @file k_median_solution.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-03-08
 */
#ifndef K_MEDIAN_SOLUTION_HPP
#define K_MEDIAN_SOLUTION_HPP

#include "paal/utils/functors.hpp"
#include "paal/data_structures/facility_location/facility_location_solution.hpp"

namespace paal {
namespace data_structures {

    /**
     * @brief solution for k median problem
     *
     * @tparam VoronoiType
     */
template <typename VoronoiType>
class KMedianSolution :
    public data_structures::FacilityLocationSolution<utils::ReturnZeroFunctor, VoronoiType> {
    typedef data_structures::FacilityLocationSolution<utils::ReturnZeroFunctor, VoronoiType> base;
public:
    /**
     * @brief constructor
     *
     * @param voronoi
     * @param uf
     * @param k
     */
    KMedianSolution(VoronoiType voronoi,
                    typename base::UnchosenFacilitiesSet uf, int k) :
        base(std::move(voronoi), std::move(uf), m_zeroFunc) {assert(int(base::getChosenFacilities().size()) == k);}
private:
    utils::ReturnZeroFunctor m_zeroFunc;

};


} //data_structures

namespace data_structures {
    /**
     * @brief specialization of FacilityLocationSolutionTraits
     *
     * @tparam Voronoi
     */
    template <typename Voronoi>
    class FacilityLocationSolutionTraits<data_structures::KMedianSolution< Voronoi>> {
        typedef VoronoiTraits<Voronoi> VT;
        typedef data_structures::KMedianSolution< Voronoi> KMS;
    public:
        typedef typename VT::VertexType VertexType;
        typedef typename VT::DistanceType Dist;
        typedef typename VT::GeneratorsSet ChosenFacilitiesSet;
        ///unchosen facilities set type
        typedef puretype(std::declval<KMS>().getUnchosenFacilities()) UnchosenFacilitiesSet;
    };
}

} //paal


#endif /* K_MEDIAN_SOLUTION_HPP */
