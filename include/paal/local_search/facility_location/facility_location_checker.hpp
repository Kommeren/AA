/**
 * @file facility_location_checker.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */

namespace paal {
namespace local_search {
namespace facility_location {

template <typename VertexType> class FacilityLocationChecker {
public:
        template <class Solution, class Update> 
    int operator()(const Solution & s, 
            const  typename utils::SolToElem<Solution>::type & se,  //SolutionElement 
            const Update & ue) {
        auto const & FLS = s.get();
        typedef typename std::decay<decltype(FLS)>::type::ObjectType FLS_T;
        int ret;
        switch (ue.getImpl()->getType()) {
            case REMOVE : {
                auto r = static_cast<const Remove<VertexType> *>(ue.getImpl());
                ret = FLS.invokeOnCopy(&FLS_T::remFacility, r->get());
                FLS.invokeOnCopy(&FLS_T::addFacility, r->get());
                break;
            }
            case ADD: {
                auto a = static_cast<const Add<VertexType> *>(ue.getImpl());
                ret = FLS.invokeOnCopy(&FLS_T::addFacility, a->get());
                FLS.invokeOnCopy(&FLS_T::remFacility, a->get());
                break;
            }
            case SWAP: {
                auto s = static_cast<const Swap<VertexType> *>(ue.getImpl());
                ret  = FLS.invokeOnCopy(&FLS_T::addFacility, s->getTo());
                ret += FLS.invokeOnCopy(&FLS_T::remFacility, s->getFrom());
                FLS.invokeOnCopy(&FLS_T::addFacility, s->getFrom());
                FLS.invokeOnCopy(&FLS_T::remFacility, s->getTo());
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
