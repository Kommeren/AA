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

template <typename VertexType> class FacilityLocationChecker {
public:
        template <class Solution, class Update> 
    auto operator()(const Solution & s, 
            const  typename utils::SolToElem<Solution>::type & se,  //SolutionElement 
            const Update & ue) ->
                typename data_structures::FacilityLocationSolutionTraits<puretype(s.get())>::Dist {
        auto const & FLS = s.get();
        typedef typename std::decay<decltype(FLS)>::type::ObjectType FLS_T;
        typename data_structures::FacilityLocationSolutionTraits<puretype(s.get())>::Dist ret, back;
        switch (ue.getImpl()->getType()) {
            case REMOVE : {
                auto r = static_cast<const Remove<VertexType> *>(ue.getImpl());
                ret = FLS.invokeOnCopy(&FLS_T::remFacility, r->get());
                //TODO for capacitated version we should  just restart copy
                back = FLS.invokeOnCopy(&FLS_T::addFacility, r->get());
                assert(ret == -back);
                break;
            }
            case ADD: {
                auto a = static_cast<const Add<VertexType> *>(ue.getImpl());
                ret = FLS.invokeOnCopy(&FLS_T::addFacility, a->get());
                back = FLS.invokeOnCopy(&FLS_T::remFacility, a->get());
                assert(ret == -back);
                break;
            }
            case SWAP: {
                auto s = static_cast<const Swap<VertexType> *>(ue.getImpl());
                ret  = FLS.invokeOnCopy(&FLS_T::addFacility, s->getTo());
                ret += FLS.invokeOnCopy(&FLS_T::remFacility, s->getFrom());
                back = FLS.invokeOnCopy(&FLS_T::addFacility, s->getFrom());
                back += FLS.invokeOnCopy(&FLS_T::remFacility, s->getTo());
                assert(ret == -back);
                break;
            }
            default: {
                assert(false);
            }
        }
        return -ret;

    }
};


} // facility_location
} // local_search
} // paal
