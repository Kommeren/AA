namespace paal {
namespace local_search {
namespace facility_location {

template <typename VertexType> class FacilityLocationUpdater {
public:
        template <typename Solution, typename UpdateElement> 
    void update(Solution & s, 
            const  typename SolToElem<decltype(std::declval<Solution>().getFacilityLocationSolution())>::type & se,  //SolutionElement 
            const UpdateElement & ue) {
        auto FLS = s.getFacilityLocationSolution();
        switch (ue.getImpl()->getType()) {
            case REMOVE :
                auto r = static_cast<Remove<VertexType> *>(ue.getImpl());
                FLS.remFacility(r->get());
                break;
            case ADD:
                auto a = static_cast<Add<VertexType> *>(ue.getImpl());
                FLS.addFacility(a->get());
                break;
            case SWAP:
                auto s = static_cast<Remove<VertexType> *>(ue.getImpl());
                FLS.addFacility(s->getTo());
                FLS.remFacility(s->getFrom());
                break;
            default:
                assert(false);
        }
    }
};


} // facility_location
} // local_search
} // paal
