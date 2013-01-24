#include "facility_location_update_element.hpp"

namespace paal {
namespace local_search {
namespace facility_location {

template <typename VertexType> class FacilityLocationChecker {
public:
        template <class Solution> 
    int checkIfImproved(Solution & s, 
            const  typename SolToElem<decltype(std::declval<Solution>().getFacilityLocationSolution())>::type & se,  //SolutionElement 
            const UpdateElement & ue) {
        auto FLS = s.getFacilityLocationSolution();
        int ret;
        switch (ue.getImpl()->getType()) {
            case REMOVE :
                auto r = static_cast<Remove<VertexType> *>(ue.getImpl());
                ret = FLS.remFacility(r->get());
                FLS.addFacility(r->get());
                break;
            case ADD:
                auto a = static_cast<Add<VertexType> *>(ue.getImpl());
                ret = FLS.addFacility(a->get());
                FLS.remFacility(a->get());
                break;
            case SWAP:
                auto s = static_cast<Remove<VertexType> *>(ue.getImpl());
                ret = FLS.addFacility(s->getTo());
                ret = FLS.remFacility(s->getFrom());
                FLS.addFacility(s->getFrom());
                FLS.remFacility(s->getTo());
                break;
            default:
                assert(false);
        }
        return -ret;

    }
};


} // facility_location
} // local_search
} // paal
