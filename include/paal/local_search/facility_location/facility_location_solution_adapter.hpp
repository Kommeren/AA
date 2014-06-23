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

#include "paal/utils/type_functions.hpp"
#include "paal/utils/functors.hpp"
#include "paal/data_structures/collection_starts_from_last_change.hpp"
#include "paal/data_structures/facility_location/facility_location_solution_traits.hpp"

#include <boost/range/algorithm/copy.hpp>
#include <boost/range/distance.hpp>
#include <boost/range/algorithm/find.hpp>

#include <unordered_map>
#include <unordered_set>

namespace paal {
namespace local_search {
namespace facility_location {

/**
 * @brief facility_location_solution adapter
 *          chosen range and unchosen range must be joined into one homogenus
* collection of Facilities.
 *
 * @tparam facility_location_solution
 */
template <typename facility_location_solution>
class facility_location_solution_adapter {
    typedef facility_location_solution FLS;

    template <typename Collection>
    auto get_cycledCopy(const Collection &col, std::size_t index) const
        ->decltype(boost::join(
              boost::make_iterator_range(
                  std::declval<typename Collection::const_iterator>(),
                  std::declval<typename Collection::const_iterator>()),
              boost::make_iterator_range(
                  std::declval<typename Collection::const_iterator>(),
                  std::declval<typename Collection::const_iterator>()))) {
        return boost::join(
            boost::make_iterator_range(col.begin() + index, col.end()),
            boost::make_iterator_range(col.begin(), col.begin() + index));
    }

  public:
    typedef typename facility_location_solution::VertexType VertexType;
    /// type of Chosen collection
    typedef decltype(std::declval<FLS>().get_chosen_facilities()) Chosen;
    /// type of Unchosen collection
    typedef decltype(std::declval<FLS>().get_unchosen_facilities()) Unchosen;
    typedef typename data_structures::facility_location_solution_traits<
        FLS>::Dist Dist;
    typedef std::vector<VertexType> UnchosenCopy;
    typedef std::vector<VertexType> ChosenCopy;

    /**
     * @brief constructor creates cycled range of all facilities
     *
     * @param sol
     */
    facility_location_solution_adapter(facility_location_solution &sol)
        : m_sol(sol), m_unchosen_copy(m_sol.get_unchosen_facilities().begin(),
                                      m_sol.get_unchosen_facilities().end()),
          m_chosen_copy(m_sol.get_chosen_facilities().begin(),
                        m_sol.get_chosen_facilities().end()),
          m_last_used_unchosen{}, m_last_used_chosen{} {}

    /**
     * @brief adds facility tentatively (used in gain computation).
     *
     * @param v
     *
     * @return
     */
    Dist add_facility_tentative(VertexType v) { return m_sol.add_facility(v); }

    /**
     * @brief adds facility
     *
     * @param v
     *
     * @return
     */
    Dist add_facility(VertexType v) {
        auto ret = add_facility_tentative(v);
        auto elemIter = boost::find(m_unchosen_copy, v);
        assert(elemIter != m_unchosen_copy.end());
        elemIter = m_unchosen_copy.erase(elemIter);
        m_last_used_unchosen = elemIter - m_unchosen_copy.begin();
        m_chosen_copy.push_back(v);
        return ret;
    }

    /**
     * @brief ads facility tentatively (used in gain computation)
     *
     * @param v
     *
     * @return
     */
    Dist remove_facility_tentative(VertexType v) {
        return m_sol.rem_facility(v);
    }

    /**
     * @brief removes facility
     *
     * @param v
     *
     * @return
     */
    Dist remove_facility(VertexType v) {
        auto ret = remove_facility_tentative(v);
        m_unchosen_copy.push_back(v);
        auto elemIter = boost::find(m_chosen_copy, v);
        assert(elemIter != m_chosen_copy.end());
        elemIter = m_chosen_copy.erase(elemIter);
        m_last_used_chosen = elemIter - m_chosen_copy.begin();
        return ret;
    }

    /**
     * @brief get solution
     *
     * @return
     */
    facility_location_solution &getfacility_location_solution() {
        return m_sol;
    }

    /**
     * @brief gets solution
     *
     * @return
     */
    const facility_location_solution &getfacility_location_solution() const {
        return m_sol;
    }

    /**
     * @brief returns copy of unchosen facilities
     *
     * @return
     */
    auto getUnchosenCopy() const->decltype(
        std::declval<facility_location_solution_adapter>().get_cycledCopy(
            UnchosenCopy{}, std::size_t{})) {
        return get_cycledCopy(m_unchosen_copy, m_last_used_unchosen);
    }

    /**
     * @brief
     *
     * @brief returns copy of chosen facilities
     *
     * @return
     */
    auto getChosenCopy() const->decltype(
        std::declval<facility_location_solution_adapter>().get_cycledCopy(
            ChosenCopy{}, std::size_t{})) {
        return get_cycledCopy(m_chosen_copy, m_last_used_chosen);
    }

  private:

    facility_location_solution &m_sol;
    /// copy of all unchosen facilities
    UnchosenCopy m_unchosen_copy;
    /// copy of all chosen facilities
    ChosenCopy m_chosen_copy;
    /// index of last facility removed from unchosen
    std::size_t m_last_used_unchosen;
    /// index of last facility removed from chosen
    std::size_t m_last_used_chosen;
};

} // facility_location
} // local_search
} // paal

#endif // FACILITY_LOCATION_SOLUTION_ADAPTER_HPP
