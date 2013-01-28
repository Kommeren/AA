
namespace paal {
namespace local_search {
namespace facility_location {

template <typename VertexType> class FacilityLocationChecker {
public:
        template <class Solution, class Update> 
    int gain(Solution & s, 
            const  typename SolToElem<Solution>::type & se,  //SolutionElement 
            const Update & ue) {
        auto FLS = s.get();
        int ret;
        switch (ue.getImpl()->getType()) {
            case REMOVE : {
                auto r = static_cast<const Remove<VertexType> *>(ue.getImpl());
                ret = FLS.remFacility(r->get());
                FLS.addFacility(r->get());
                break;
            }
            case ADD: {
                auto a = static_cast<const Add<VertexType> *>(ue.getImpl());
                ret = FLS.addFacility(a->get());
                FLS.remFacility(a->get());
                break;
            }
            case SWAP: {
                auto s = static_cast<const Swap<VertexType> *>(ue.getImpl());
                ret = FLS.addFacility(s->getTo());
                ret += FLS.remFacility(s->getFrom());
                FLS.addFacility(s->getFrom());
                FLS.remFacility(s->getTo());
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
