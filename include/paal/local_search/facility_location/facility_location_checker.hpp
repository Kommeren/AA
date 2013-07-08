/**
 * @file facility_location_checker.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */

#include <cassert>

#include "paal/utils/type_functions.hpp"
#include "facility_location_update_element.hpp"
#include "facility_location_solution_element.hpp"
#include "paal/data_structures/facility_location/facility_location_solution_traits.hpp"

namespace paal {
namespace local_search {
namespace facility_location {

template <typename VertexType> 
class FacilityLocationCheckerRemove {
public:
        template <class Solution> 
    auto operator()(const Solution & s, 
            const  typename utils::SolToElem<Solution>::type & se,  //SolutionElement 
            Remove<VertexType> r) ->
                typename data_structures::FacilityLocationSolutionTraits<puretype(s.get())>::Dist {
        auto const & FLS = s.get();
        typedef typename std::decay<decltype(FLS)>::type::ObjectType FLS_T;

        typename data_structures::FacilityLocationSolutionTraits<puretype(s.get())>::Dist ret, back;

        ret = FLS.invokeOnCopy(&FLS_T::remFacility, r.get());
        //TODO for capacitated version we should  just restart copy
        back = FLS.invokeOnCopy(&FLS_T::addFacility, r.get());
        assert(ret == -back);
        return -ret;
    }
};

template <typename VertexType> 
class FacilityLocationCheckerAdd {
public:
        template <class Solution> 
    auto operator()(const Solution & s, 
            const  typename utils::SolToElem<Solution>::type & se,  //SolutionElement 
            Add<VertexType> a) ->
                typename data_structures::FacilityLocationSolutionTraits<puretype(s.get())>::Dist {
        auto const & FLS = s.get();
        typedef typename std::decay<decltype(FLS)>::type::ObjectType FLS_T;
        typename data_structures::FacilityLocationSolutionTraits<puretype(s.get())>::Dist ret, back;

        ret = FLS.invokeOnCopy(&FLS_T::addFacility, a.get());
        back = FLS.invokeOnCopy(&FLS_T::remFacility, a.get());
        assert(ret == -back);
        return -ret;

    }
};

template <typename VertexType> 
class FacilityLocationCheckerSwap {
public:
        template <class Solution> 
    auto operator()(const Solution & sol, 
            const  typename utils::SolToElem<Solution>::type & se,  //SolutionElement 
            const Swap<VertexType> & s) ->
                typename data_structures::FacilityLocationSolutionTraits<puretype(sol.get())>::Dist {
        auto const & FLS = sol.get();
        typedef typename std::decay<decltype(FLS)>::type::ObjectType FLS_T;
        typename data_structures::FacilityLocationSolutionTraits<puretype(sol.get())>::Dist ret, back;
        
        ret  = FLS.invokeOnCopy(&FLS_T::addFacility, s.getTo());
        ret += FLS.invokeOnCopy(&FLS_T::remFacility, s.getFrom());
        back = FLS.invokeOnCopy(&FLS_T::addFacility, s.getFrom());
        back += FLS.invokeOnCopy(&FLS_T::remFacility, s.getTo());
        assert(ret == -back);
        return -ret;
    }
};


} // facility_location
} // local_search
} // paal
