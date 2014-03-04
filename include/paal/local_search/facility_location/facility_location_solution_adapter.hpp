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
#include  <boost/range/algorithm/find.hpp>

#include "paal/utils/type_functions.hpp"
#include "paal/utils/functors.hpp"
#include "paal/data_structures/collection_starts_from_last_change.hpp"
#include "paal/data_structures/facility_location/facility_location_solution_traits.hpp"

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

    template <typename Collection>
    auto getCycledCopy(const Collection & col, std::size_t index) const ->
        decltype(boost::join(
                  boost::make_iterator_range(std::declval<typename Collection::const_iterator>(), std::declval<typename Collection::const_iterator>())
                , boost::make_iterator_range(std::declval<typename Collection::const_iterator>(), std::declval<typename Collection::const_iterator>())
                )) {
        return boost::join(
                boost::make_iterator_range(col.begin() + index, col.end())
              , boost::make_iterator_range(col.begin(), col.begin() + index)
                );
    }

public:
    typedef typename FacilityLocationSolution::VertexType VertexType;
    ///type of Chosen collection
    typedef decltype(std::declval<FLS>().getChosenFacilities()) Chosen;
    ///type of Unchosen collection
    typedef decltype(std::declval<FLS>().getUnchosenFacilities()) Unchosen;
    typedef typename data_structures::FacilityLocationSolutionTraits<FLS>::Dist Dist;
    typedef std::vector<VertexType> UnchosenCopy;
    typedef std::vector<VertexType> ChosenCopy;

    /**
     * @brief constructor creates cycled range of all facilities
     *
     * @param sol
     */
    FacilityLocationSolutionAdapter(FacilityLocationSolution & sol) :
            m_sol(sol),
            m_unchosenCopy(m_sol.getUnchosenFacilities().begin(),
                         m_sol.getUnchosenFacilities().end()),
            m_chosenCopy(m_sol.getChosenFacilities().begin(),
                         m_sol.getChosenFacilities().end()),
            m_lastUsedUnchosen{},
            m_lastUsedChosen{}
    {}

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
     * @param v
     *
     * @return
     */
    Dist addFacility(VertexType v) {
        auto ret = addFacilityTentative(v);
        auto elemIter = boost::find(m_unchosenCopy, v);
        assert(elemIter != m_unchosenCopy.end());
        elemIter = m_unchosenCopy.erase(elemIter);
        m_lastUsedUnchosen = elemIter - m_unchosenCopy.begin();
        m_chosenCopy.push_back(v);
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
     * @param v
     *
     * @return
     */
    Dist removeFacility(VertexType v) {
        auto ret = removeFacilityTentative(v);
        m_unchosenCopy.push_back(v);
        auto elemIter = boost::find(m_chosenCopy, v);
        assert(elemIter != m_chosenCopy.end());
        elemIter = m_chosenCopy.erase(elemIter);
        m_lastUsedChosen = elemIter - m_chosenCopy.begin();
        return ret;
    }

    /**
     * @brief get solution
     *
     * @return
     */
    FacilityLocationSolution & getFacilityLocationSolution() {
        return m_sol;
    }

    /**
     * @brief gets solution
     *
     * @return
     */
    const FacilityLocationSolution & getFacilityLocationSolution() const {
        return m_sol;
    }


    /**
     * @brief returns copy of unchosen facilities
     *
     * @return
     */
    auto getUnchosenCopy() const ->
        decltype(std::declval<FacilityLocationSolutionAdapter>().getCycledCopy(UnchosenCopy{}, std::size_t{})) {
            return getCycledCopy(m_unchosenCopy, m_lastUsedUnchosen);
    }

    /**
     * @brief
     *
     * @brief returns copy of chosen facilities
     *
     * @return
     */
    auto getChosenCopy() const ->
        decltype(std::declval<FacilityLocationSolutionAdapter>().getCycledCopy(ChosenCopy{}, std::size_t{})) {
            return getCycledCopy(m_chosenCopy, m_lastUsedChosen);
    }

private:

    FacilityLocationSolution & m_sol;
    ///copy of all unchosen facilities
    UnchosenCopy m_unchosenCopy;
    ///copy of all chosen facilities
    ChosenCopy m_chosenCopy;
    ///index of last facility removed from unchosen
    std::size_t m_lastUsedUnchosen;
    ///index of last facility removed from chosen
    std::size_t m_lastUsedChosen;
};


} //facility_location
} // local_search
} // paal

#endif //FACILITY_LOCATION_SOLUTION_ADAPTER_HPP
