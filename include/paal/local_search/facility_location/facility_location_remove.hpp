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
#include "paal/local_search/facility_location/facility_location_solution_element.hpp"


namespace paal {
namespace local_search {
namespace facility_location {

    /**
     * @brief Remove move type
     */
struct Remove {};

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
     * @param se
     *
     * @return
     */
        template <class Solution>
    auto operator()(Solution & s,
            const  typename utils::CollectionToElem<Solution>::type & se,  //SolutionElement
            Remove) ->
                typename data_structures::FacilityLocationSolutionTraits<puretype(s.get())>::Dist {

        typename data_structures::FacilityLocationSolutionTraits<puretype(s.get())>::Dist ret, back;

        ret = s.removeFacilityTentative(se.getElem());
        //TODO for capacitated version we should  just restart copy
        back = s.addFacilityTentative(se.getElem());
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
     * @param se
     */
        template <typename Solution>
    bool operator()(
            Solution & s,
            typename utils::CollectionToElem<Solution>::type & se,  //SolutionElement
            Remove) {
        s.removeFacility(se);
        return true;
    }
};

/**
 * @brief get moves functor for facility location remove
 *
 * @tparam VertexType
 */
template <typename VertexType>
class FacilityLocationGetMovesRemove {
    typedef std::vector<Remove> Moves;
    typedef typename Moves::iterator Iter;

public:

    /**
     * @brief constructor
     */
    FacilityLocationGetMovesRemove() : m_remove(1) {}

    /**
     * @brief operator()
     *
     * @tparam Solution
     * @param el
     *
     * @return
     */
    template <typename Solution>
        typename std::pair<Iter, Iter>
    operator()(const Solution &,
            typename utils::CollectionToElem<Solution>::type & el) {
        if(el.getIsChosen() == CHOSEN) {
            //the move of CHOSEN could be remove
            return std::make_pair(m_remove.begin(), m_remove.end());
        }
        return std::pair<Iter, Iter>();
    }
private:
    Moves m_remove;
};

} // facility_location
} // local_search
} // paal

#endif /* FACILITY_LOCATION_REMOVE_HPP */
