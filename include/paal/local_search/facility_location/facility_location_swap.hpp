/**
 * @file facility_location_swap.hpp
* @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-07-08
 */
#ifndef FACILITY_LOCATION_SWAP_HPP
#define FACILITY_LOCATION_SWAP_HPP
#include <cassert>
#include <vector>
#include <numeric>
#include <cstdlib>

#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/iterator/transform_iterator.hpp>

#include "paal/data_structures/facility_location/facility_location_solution_traits.hpp"
#include "paal/utils/type_functions.hpp"
#include "paal/data_structures/combine_iterator.hpp"

namespace paal {
namespace local_search {
namespace facility_location {

    /**
     * @brief swap type
     *
     * @tparam T
     */
template <typename T> class Swap {
public:
    /**
     * @brief constructor
     *
     * @param from
     * @param to
     */
    Swap(T from, T to) : m_from(from), m_to(to) {}

    Swap() = default;

    /**
     * @brief from getter
     *
     * @return
     */
    T getFrom() const {
        return m_from;
    }

    /**
     * @brief to getter
     *
     * @return
     */
    T getTo() const {
        return m_to;
    }

    /**
     * @brief form setter
     *
     * @param from
     */
    void setFrom(T from) {
        m_from = from;
    }

    /**
     * @brief from setter
     *
     * @param to
     */
    void setTo(T to) {
        m_to = to;
    }

private:
    T m_from;
    T m_to;
};

///operator() creates Swap  from (from, to)
struct MakeSwap {
    ///operator()
    template <typename T>
    Swap<T>
    operator()(T from, T to) const {
        return Swap<T>(from, to);
    }
};

/**
 * @brief gain functor for swap in facility location problem.
 *
 * @tparam VertexType
 */
template <typename VertexType>
struct FacilityLocationGainSwap {
    /**
     * @brief operator()
     *
     * @tparam Solution
     * @param sol
     * @param s
     *
     * @return
     */
        template <class Solution>
    auto operator()(Solution & sol,
            const Swap<VertexType> & s) ->
                typename data_structures::FacilityLocationSolutionTraits<puretype(sol.getFacilityLocationSolution())>::Dist {

        typename data_structures::FacilityLocationSolutionTraits<puretype(sol.getFacilityLocationSolution())>::Dist ret, back;

        ret   = sol.addFacilityTentative(s.getTo());
        ret  += sol.removeFacilityTentative(s.getFrom());
        back  = sol.addFacilityTentative(s.getFrom());
        back += sol.removeFacilityTentative(s.getTo());
        assert(ret == -back);
        return -ret;
    }
};


/**
 * @brief commit functor for facility location problem
 *
 * @tparam VertexType
 */
template <typename VertexType>
struct FacilityLocationCommitSwap {
    /**
     * @brief operator()
     *
     * @tparam Solution
     * @param sol
     * @param s
     */
    template <typename Solution>
    bool operator()(Solution & sol,
            const Swap<VertexType> & s) {
        sol.addFacility(s.getTo());
        sol.removeFacility(s.getFrom());
        return true;
    }
};

/**
 * @brief get moves functor for facility location problem
 *
 * @tparam VertexType
 */
template <typename VertexType>
struct FacilityLocationGetMovesSwap {

    ///operator()
    template <typename Solution>
    auto operator()(const Solution &s) ->
        boost::iterator_range<data_structures::CombineIterator<MakeSwap
                    , puretype(s.getChosenCopy())
                    , puretype(s.getUnchosenCopy())>>

        //this does NOT work on clang-3.4!!!
    /*    std::pair<
        decltype(data_structures::make_CombineIterator(MakeSwap{}
                , s.getUnchosenCopy()
                , s.getChosenCopy())),
        decltype(data_structures::make_CombineIterator(MakeSwap{}
                , s.getUnchosenCopy()
                , s.getChosenCopy()))>*/
    {
        auto begin = data_structures::make_CombineIterator(MakeSwap{}
                , s.getChosenCopy()
                , s.getUnchosenCopy()
                );
        decltype(begin) end;
        return boost::make_iterator_range(begin, end);
    }
};

} // facility_location
} // local_search
} // paal

#endif /* FACILITY_LOCATION_SWAP_HPP */
