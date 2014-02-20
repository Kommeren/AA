/**
 * @file facility_location_solution.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */

#ifndef FACILITY_LOCATION_SOLUTION_HPP
#define FACILITY_LOCATION_SOLUTION_HPP

#define BOOST_RESULT_OF_USE_DECLTYPE

#include <unordered_set>
#include <cassert>
#include <type_traits>

#include "paal/data_structures/voronoi/voronoi.hpp"

#include "facility_location_solution_traits.hpp"

namespace paal {
namespace data_structures {

/**
 * @brief describes solution to facility location
 * The initial solution is passed as voronoi, which has to be the model of the \ref voronoi concept.
 * The generators of the voronoi are the facilities and the vertices are the clients.
 *
 * @tparam FacilityCost
 * @tparam VoronoiType
 */
template <typename FacilityCost, typename VoronoiType>
class FacilityLocationSolution {
    public:
        typedef VoronoiTraits<VoronoiType> VT;
        typedef typename VT::VertexType VertexType;
        typedef typename VT::DistanceType Dist;
        typedef typename VT::GeneratorsSet ChosenFacilitiesSet;
        typedef std::unordered_set<VertexType, boost::hash<VertexType>> UnchosenFacilitiesSet;

        /**
         * @brief constructor
         *
         * @param voronoi
         * @param uf
         * @param c
         */
        FacilityLocationSolution(VoronoiType  voronoi,
                                 UnchosenFacilitiesSet uf,
                                 const FacilityCost & c) :
            m_voronoi(std::move(voronoi)), m_unchosenFacilities(std::move(uf)), m_facCosts(c) {}

        /// returns diff between new cost and old cost
        Dist addFacility(VertexType f) {
            assert(m_unchosenFacilities.find(f) != m_unchosenFacilities.end());
            m_unchosenFacilities.erase(f);

            return  m_facCosts(f) + m_voronoi.addGenerator(f);
        }

        /// returns diff between new cost and old cost
        Dist remFacility(VertexType f) {
            assert(m_unchosenFacilities.find(f) == m_unchosenFacilities.end());
            m_unchosenFacilities.insert(f);

            return -m_facCosts(f) + m_voronoi.remGenerator(f);
        }

        ///getter for unchosen facilities
        const UnchosenFacilitiesSet & getUnchosenFacilities() const {
            return m_unchosenFacilities;
        }

        ///setter for unchosen facilities
        const ChosenFacilitiesSet & getChosenFacilities() const {
            return m_voronoi.getGenerators();
        }

        ///gets clients assigned to specific facility
        decltype(std::declval<VoronoiType>().getVerticesForGenerator(std::declval<VertexType>()))
        getClientsForFacility(VertexType f) const {
            return m_voronoi.getVerticesForGenerator(f);
        }

        /// gets voronoi
        const VoronoiType & getVoronoi() const {
            return m_voronoi;
        }

    private:

        VoronoiType m_voronoi;
        UnchosenFacilitiesSet m_unchosenFacilities;
        const FacilityCost & m_facCosts;
};

/**
 * @brief traits for FacilityLocationSolution
 *
 * @tparam FacilityCost
 * @tparam Voronoi
 */
template <typename FacilityCost, typename Voronoi>
class FacilityLocationSolutionTraits<FacilityLocationSolution<FacilityCost, Voronoi>> {
    typedef VoronoiTraits<Voronoi> VT;
    typedef FacilityLocationSolution<FacilityCost, Voronoi> FLS;
public:
    typedef typename VT::VertexType VertexType;
    typedef typename VT::DistanceType Dist;
    typedef typename VT::GeneratorsSet ChosenFacilitiesSet;
    ///unchosen facilities set
    typedef puretype(std::declval<FLS>().getUnchosenFacilities()) UnchosenFacilitiesSet;
};


};
};

#endif //FACILITY_LOCATION_SOLUTION_HPP
