/**
 * @file facility_location_swap.hpp
* @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-07-08
 */
#ifndef FACILITY_LOCATION_SWAP_HPP
#define FACILITY_LOCATION_SWAP_HPP

#include "paal/data_structures/facility_location/facility_location_solution_traits.hpp"
#include "paal/utils/type_functions.hpp"
#include "paal/data_structures/combine_iterator.hpp"

#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/iterator/transform_iterator.hpp>

#include <cassert>
#include <vector>
#include <numeric>
#include <cstdlib>

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
    T get_from() const {
        return m_from;
    }

    /**
     * @brief to getter
     *
     * @return
     */
    T get_to() const {
        return m_to;
    }

    /**
     * @brief form setter
     *
     * @param from
     */
    void set_from(T from) {
        m_from = from;
    }

    /**
     * @brief from setter
     *
     * @param to
     */
    void set_to(T to) {
        m_to = to;
    }

private:
    T m_from;
    T m_to;
};

///operator() creates Swap  from (from, to)
struct make_swap {
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
struct facility_location_gain_swap {
    /**
     * @brief operator()
     *
     * @tparam Solution
     * @param sol
     * @param s
     *
     * @return
     */
        template <class Solution, class VertexType>
    auto operator()(Solution & sol,
            const Swap<VertexType> & s) ->
                typename data_structures::facility_location_solution_traits<puretype(sol.getfacility_location_solution())>::Dist {

        typename data_structures::facility_location_solution_traits<puretype(sol.getfacility_location_solution())>::Dist ret, back;

        ret   = sol.add_facility_tentative(s.get_to());
        ret  += sol.remove_facility_tentative(s.get_from());
        back  = sol.add_facility_tentative(s.get_from());
        back += sol.remove_facility_tentative(s.get_to());
        assert(ret == -back);
        return -ret;
    }
};


/**
 * @brief commit functor for facility location problem
 */
struct facility_location_commit_swap {
    /**
     * @brief operator()
     *
     * @tparam Solution
     * @param sol
     * @param s
     */
    template <typename Solution, typename VertexType>
    bool operator()(Solution & sol,
            const Swap<VertexType> & s) {
        sol.add_facility(s.get_to());
        sol.remove_facility(s.get_from());
        return true;
    }
};

/**
 * @brief get moves functor for facility location problem
 */
struct facility_locationget_moves_swap {

    ///operator()
    template <typename Solution>
    auto operator()(const Solution &s) ->
        boost::iterator_range<data_structures::combine_iterator<make_swap
                    , puretype(s.getChosenCopy())
                    , puretype(s.getUnchosenCopy())>>

        //this does NOT work on clang-3.4!!!
    /*    std::pair<
        decltype(data_structures::make_combine_iterator(make_swap{}
                , s.getUnchosenCopy()
                , s.getChosenCopy())),
        decltype(data_structures::make_combine_iterator(make_swap{}
                , s.getUnchosenCopy()
                , s.getChosenCopy()))>*/
    {
        auto begin = data_structures::make_combine_iterator(make_swap{}
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
