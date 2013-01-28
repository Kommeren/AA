namespace paal {
namespace local_search {
namespace facility_location {

template <typename VertexType> class FacilityLocationUpdater {
public:
        template <typename Solution, typename Update> 
    void update(Solution & s, 
            const  typename SolToElem<Solution>::type & se,  //SolutionElement 
            const Update & u) {
        auto FLS = s.get();
        switch (u.getImpl()->getType()) {
            case REMOVE : {
                auto r = static_cast<const Remove<VertexType> *>(u.getImpl());
                FLS.remFacility(r->get());
                break;
            }
            case ADD: {
                auto a = static_cast<const Add<VertexType> *>(u.getImpl());
                FLS.addFacility(a->get());
                break;
            }
            case SWAP: {
                auto s = static_cast<const Swap<VertexType> *>(u.getImpl());
                FLS.addFacility(s->getTo());
                FLS.remFacility(s->getFrom());
                break;
            }
            default: {
                assert(false);\
            }
        }
    }
};


} // facility_location
} // local_search
} // paal
