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

#include "paal/utils/iterator_utils.hpp"
#include "paal/utils/type_functions.hpp"
#include "paal/data_structures/facility_location/facility_location_solution_traits.hpp"
#include "paal/local_search/facility_location/facility_location_solution_element.hpp"


namespace paal {
namespace local_search {
namespace facility_location {

struct Remove {};

template <typename VertexType> 
class FacilityLocationCheckerRemove {
public:
        template <class Solution> 
    auto operator()(Solution & s, 
            const  typename utils::SolToElem<Solution>::type & se,  //SolutionElement 
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

template <typename VertexType> 
class FacilityLocationUpdaterRemove {
public:
        template <typename Solution> 
    void operator()(
            Solution & s, 
            typename utils::SolToElem<Solution>::type & se,  //SolutionElement 
            Remove) {
        s.removeFacility(se);
    }
};

template <typename VertexType> 
class FacilityLocationGetNeighborhoodRemove {
    typedef std::vector<Remove> Updates;
    typedef typename Updates::iterator Iter;

public: 

    FacilityLocationGetNeighborhoodRemove() : m_remove(1) {}

    template <typename Solution> 
        typename std::pair<Iter, Iter>
    operator()(const Solution &, 
            typename utils::SolToElem<Solution>::type & el) {
        if(el.getIsChosen() == CHOSEN) { 
            //the update of CHOSEN could be remove
            return std::make_pair(m_remove.begin(), m_remove.end());
        }
        return std::pair<Iter, Iter>();
    }
private:
    Updates m_remove;
};

} // facility_location
} // local_search
} // paal

#endif /* FACILITY_LOCATION_REMOVE_HPP */
