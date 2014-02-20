/**
 * @file facility_location_solution_adapter.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */

#ifndef FACILITY_LOCATION_SOLUTION_ADAPTER_HPP
#define FACILITY_LOCATION_SOLUTION_ADAPTER_HPP

#define BOOST_RESULT_OF_USE_DECLTYPE

#include <unordered_map>
#include <unordered_set>

#include <boost/range/algorithm/copy.hpp>
#include <boost/range/distance.hpp>
#include <boost/range/adaptor/transformed.hpp>

#include "paal/utils/type_functions.hpp"
#include "paal/utils/functors.hpp"
#include "paal/data_structures/collection_starts_from_last_change.hpp"
#include "paal/data_structures/facility_location/facility_location_solution_traits.hpp"
#include "paal/local_search/facility_location/facility_location_solution_element.hpp"

namespace paal {
namespace local_search {
namespace facility_location {


    /**
     * @brief FacilityLocationSolution adapter
     *          chosen range and unchosen range must be joined into one homogenus collection of Facilities.
     *
     * @tparam FacilityLocationSolution
     */
template <typename FacilityLocationSolution>
class FacilityLocationSolutionAdapter {
    typedef FacilityLocationSolution FLS;
public:
    typedef typename FacilityLocationSolution::VertexType VertexType;
    typedef Facility<VertexType> Fac;
    typedef std::vector<Fac> Facilities;
    typedef typename Facilities::iterator FacIter;
    typedef data_structures::CollectionStartsFromLastChange<FacIter, FacilityHash> CycledFacilities;
    typedef typename CycledFacilities::ResultIterator ResultIterator;
    ///type of Chosen collection
    typedef decltype(std::declval<FLS>().getChosenFacilities()) Chosen;
    ///type of Unchosen collection
    typedef decltype(std::declval<FLS>().getUnchosenFacilities()) Unchosen;
    typedef typename data_structures::FacilityLocationSolutionTraits<FLS>::Dist Dist;
    typedef std::unordered_set<VertexType> UnchosenCopy;
public:

    /**
     * @brief constructor creates cycled range of all facilities
     *
     * @param sol
     */
    FacilityLocationSolutionAdapter(FacilityLocationSolution & sol) :
            m_sol(sol),
            m_unchosenCopy(m_sol.getUnchosenFacilities().begin(),
                         m_sol.getUnchosenFacilities().end()) {
        auto const &  ch = m_sol.getChosenFacilities();
        auto const &  uch = m_sol.getUnchosenFacilities();

        auto transformChosen = [](VertexType f){ return Fac(CHOSEN ,f);};
        auto transformUnchosen = [](VertexType f){ return Fac(UNCHOSEN, f);};
        auto transformChosenAssignable = utils::make_AssignableFunctor(transformChosen);
        auto transformUnchosenAssignable = utils::make_AssignableFunctor(transformUnchosen);

        auto transformedChosen = boost::adaptors::transform(ch, transformChosenAssignable);
        auto transformedUnchosen = boost::adaptors::transform(uch, transformUnchosenAssignable);

        auto chosenSize = boost::distance(ch);
        m_facilities.resize(chosenSize + boost::distance(uch));
        boost::copy(transformedChosen, m_facilities.begin());
        boost::copy(transformedUnchosen, m_facilities.begin() + chosenSize);

        for(auto & f : m_facilities) {
            m_vertexToFac.insert(std::make_pair(f.getElem(), &f));
        }
        m_cycledFacilities = CycledFacilities(m_facilities.begin(), m_facilities.end());
    }

    /**
     * @brief adds facility tentatively (used in gain computation).
     *
     * @param v
     *
     * @return
     */
    Dist addFacilityTentative(VertexType v) {
        return m_sol.addFacility(v);
    }

    /**
     * @brief adds facility
     *
     * @param se
     *
     * @return
     */
    Dist addFacility(Fac & se) {
        auto ret = addFacilityTentative(se.getElem());
        assert(se.getIsChosen() == UNCHOSEN);
        se.setIsChosen(CHOSEN);
        m_unchosenCopy.erase(se.getElem());
        m_cycledFacilities.setLastChange(se);
        return ret;
    }

    /**
     * @brief ads facility tentatively (used in gain computation)
     *
     * @param v
     *
     * @return
     */
    Dist removeFacilityTentative(VertexType v) {
        return m_sol.remFacility(v);
    }

    /**
     * @brief removes facility
     *
     * @param se
     *
     * @return
     */
    Dist removeFacility(Fac & se) {
        auto ret = removeFacilityTentative(se.getElem());
        assert(se.getIsChosen() == CHOSEN);
        se.setIsChosen(UNCHOSEN);
        m_unchosenCopy.insert(se.getElem());
        m_cycledFacilities.setLastChange(se);
        return ret;
    }

    /**
     * @brief get facility at vertex
     *
     * @param v
     *
     * @return
     */
    Fac & getFacility(VertexType v) {
        auto i = m_vertexToFac.find(v);
        assert(i != m_vertexToFac.end());
        return *(i->second);
    }


    /**
     * @brief begin of the facilities range
     *
     * @return
     */
    ResultIterator begin() {
        return m_cycledFacilities.begin();
    }

    /**
     * @brief end of the facilities range
     *
     * @return
     */
    ResultIterator end() {
        return m_cycledFacilities.end();
    }

    /**
     * @brief get solution
     *
     * @return
     */
    FacilityLocationSolution & get() {
        return m_sol;
    }

    /**
     * @brief gets solution
     *
     * @return
     */
    const FacilityLocationSolution & get() const {
        return m_sol;
    }

    /**
     * @brief gets
     *
     * @return
     */
    const UnchosenCopy & getUnchosenCopy() const {
        return m_unchosenCopy;
    }

private:
    FacilityLocationSolution & m_sol;
    Facilities m_facilities;
    CycledFacilities m_cycledFacilities;
    std::unordered_map<VertexType, Fac*> m_vertexToFac;
    UnchosenCopy m_unchosenCopy;
};


} //facility_location
} // local_search
} // paal

#endif //FACILITY_LOCATION_SOLUTION_ADAPTER_HPP
