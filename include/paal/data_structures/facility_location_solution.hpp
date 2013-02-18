/**
 * @file facility_location_solution.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */
#include <set>
#include <map>
#include <cassert>

#include "paal/data_structures/voronoi.hpp"

#ifndef __FACILITY_LOCATION_SOLUTION__
#define __FACILITY_LOCATION_SOLUTION__

namespace paal {
namespace data_structures {

template <typename FacilityCost, typename VoronoiType/*, typename VoronoiType::DistanceType NO_FACILITY_COST = INT_MAX*/>
class FacilityLocationSolution { 
    public:
        typedef typename VoronoiType::VertexType VertexType;
        typedef typename VoronoiType::Dist Dist;
        typedef typename VoronoiType::GeneratorsSet ChosenFacilitiesSet;
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
        
        const VoronoiType & getVoronoi() const {
            return m_voronoi;
        }

    private:

        VoronoiType m_voronoi;
        UnchosenFacilitiesSet m_unchosenFacilities;
        const FacilityCost & m_facCosts;
};


};
};

#endif //__FACILITY_LOCATION_SOLUTION__
