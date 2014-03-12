/**
 * @file facility_location_remove.hpp
* @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-07-08
 */
#ifndef FACILITY_LOCATION_REMOVE_HPP
#define FACILITY_LOCATION_REMOVE_HPP

#include <cassert>
#include <vector>
#include <numeric>
#include <cstdlib>

#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/iterator/transform_iterator.hpp>

#include "paal/utils/type_functions.hpp"
#include "paal/data_structures/facility_location/facility_location_solution_traits.hpp"


namespace paal {
namespace local_search {
namespace facility_location {

/**
 * @brief gain functor for facility location
 *
 * @tparam VertexType
 */
template <typename VertexType>
class FacilityLocationGainRemove {
public:
    /**
     * @brief operator()
     *
     * @tparam Solution
     * @param s
     * @param e
     *
     * @return
     */
        template <typename Solution, typename ChosenElement>
    auto operator()(Solution & s,
            ChosenElement e) ->
                typename data_structures::FacilityLocationSolutionTraits<puretype(s.getFacilityLocationSolution())>::Dist {

        typename data_structures::FacilityLocationSolutionTraits<puretype(s.getFacilityLocationSolution())>::Dist ret, back;

        ret = s.removeFacilityTentative(e);
        //TODO for capacitated version we should  just restart copy
        back = s.addFacilityTentative(e);
        assert(ret == -back);
        return -ret;
    }
};

/**
 * @brief commit functor for facility location
 *
 * @tparam VertexType
 */
template <typename VertexType>
class FacilityLocationCommitRemove {
public:
    /**
     * @brief operator()
     *
     * @tparam Solution
     * @param s
     * @param e
     */
        template <typename Solution, typename ChosenElement>
    bool operator()(
            Solution & s,
            ChosenElement e) {
        s.removeFacility(e);
        return true;
    }
};

/**
 * @brief get moves functor for facility location remove
 *
 * @tparam VertexType
 */
template <typename VertexType>
struct FacilityLocationGetMovesRemove {
    /**
     * @brief operator()
     *
     * @tparam Solution
     *
     * @return
     */
    template <typename Solution>
    auto operator()(const Solution & sol) ->
        decltype(sol.getChosenCopy())
    {
        return sol.getChosenCopy();
    }
};

} // facility_location
} // local_search
} // paal

#endif /* FACILITY_LOCATION_REMOVE_HPP */
