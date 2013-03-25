/**
 * @file facility_location_solution.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */

#ifndef FACILITY_LOCATION_SOLUTION_HPP
#define FACILITY_LOCATION_SOLUTION_HPP

#include <set>
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
        typedef std::set<VertexType> UnchosenFacilitiesSet;

        FacilityLocationSolution(VoronoiType  voronoi,
                                 UnchosenFacilitiesSet uf,
                                 const FacilityCost & c) :
            m_voronoi(std::move(voronoi)), m_unchosenFacilities(std::move(uf)), m_facCosts(c) {}
        
        FacilityLocationSolution(const FacilityLocationSolution & fls) :
            m_voronoi(fls.m_voronoi), m_unchosenFacilities(fls.m_unchosenFacilities), m_facCosts(fls.m_facCosts) {}
        
        FacilityLocationSolution(FacilityLocationSolution && fls) :
            m_voronoi(std::move(fls.m_voronoi)), m_unchosenFacilities(std::move(fls.m_unchosenFacilities)), m_facCosts(fls.m_facCosts) {}


       
        // returns diff between new cost and old cost
        Dist addFacility(VertexType f) {
            assert(m_unchosenFacilities.find(f) != m_unchosenFacilities.end());
            m_unchosenFacilities.erase(f);

            return  m_facCosts(f) + m_voronoi.addGenerator(f);
        }
        
        // returns diff between new cost and old cost
        Dist remFacility(VertexType f) {
            assert(m_unchosenFacilities.find(f) == m_unchosenFacilities.end());
            m_unchosenFacilities.insert(f);

            return -m_facCosts(f) + m_voronoi.remGenerator(f);
        }

        const UnchosenFacilitiesSet & getUnchosenFacilities() const {
            return m_unchosenFacilities;
        }
        
        const ChosenFacilitiesSet & getChosenFacilities() const {
            return m_voronoi.getGenerators();
        }

        decltype(std::declval<VoronoiType>().getVerticesForGenerator(std::declval<VertexType>()))
        getClientsForFacility(VertexType f) const {
            return m_voronoi.getVerticesForGenerator(f);
        }

    private:

        VoronoiType m_voronoi;
        UnchosenFacilitiesSet m_unchosenFacilities;
        const FacilityCost & m_facCosts;
};

template <typename FacilityCost, typename Voronoi>
class FacilityLocationSolutionTraits<FacilityLocationSolution<FacilityCost, Voronoi>> {
    typedef VoronoiTraits<Voronoi> VT;
    typedef FacilityLocationSolution<FacilityCost, Voronoi> FLS;
public:
    typedef typename VT::VertexType VertexType;
    typedef typename VT::DistanceType Dist;
    typedef typename VT::GeneratorsSet ChosenFacilitiesSet;
    typedef puretype(std::declval<FLS>().getUnchosenFacilities()) UnchosenFacilitiesSet;
};


};
};

#endif //FACILITY_LOCATION_SOLUTION_HPP
