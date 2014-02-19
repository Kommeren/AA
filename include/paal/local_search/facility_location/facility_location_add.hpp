/**
 * @file facility_location_add.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-07-08
 */
#ifndef FACILITY_LOCATION_ADD_HPP
#define FACILITY_LOCATION_ADD_HPP

#include <vector>
#include <numeric>
#include <cstdlib>
#include <cassert>

#include <boost/iterator/iterator_adaptor.hpp>

#include "paal/utils/type_functions.hpp"
#include "paal/data_structures/facility_location/facility_location_solution_traits.hpp"
#include "paal/utils/type_functions.hpp"
#include "paal/local_search/facility_location/facility_location_solution_element.hpp"


namespace paal {
namespace local_search {
namespace facility_location {

struct Add {};

template <typename VertexType>
class FacilityLocationCommitAdd {
public:
    template <typename Solution>
    void operator()(
            Solution & s,
            typename utils::CollectionToElem<Solution>::type & se,  //SolutionElement
            Add) {

        s.addFacility(se);
    }
};


template <typename VertexType>
class FacilityLocationGetMovesAdd {
    typedef std::vector<Add> Moves;
    typedef typename Moves::iterator Iter;

public:
    FacilityLocationGetMovesAdd() : m_add(1) {}
    typedef Facility<VertexType> Fac;

    template <typename Solution>
        std::pair<Iter, Iter>
    operator()(const Solution &, Fac & el) {
        if(el.getIsChosen() == UNCHOSEN) {
            //the move of UNCHOSEN could be added to the solution
            return std::make_pair(m_add.begin(), m_add.end());
        }
        return std::pair<Iter, Iter>();
    }
private:
    Moves m_add;
};


template <typename VertexType>
class FacilityLocationCheckerAdd {
public:
        template <class Solution>
    auto operator()(Solution & s,
            const  typename utils::CollectionToElem<Solution>::type & se,  //SolutionElement
            Add ) ->
                typename data_structures::FacilityLocationSolutionTraits<puretype(s.get())>::Dist {
        auto ret = s.addFacilityTentative(se.getElem());
        auto back = s.removeFacilityTentative(se.getElem());
        assert(ret == -back);
        return -ret;

    }
};


} // facility_location
} // local_search
} // paal

#endif /* FACILITY_LOCATION_ADD_HPP */
