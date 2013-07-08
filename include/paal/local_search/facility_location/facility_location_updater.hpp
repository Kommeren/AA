/**
 * @file facility_location_updater.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */

namespace paal {
namespace local_search {
namespace facility_location {

template <typename VertexType> 
class FacilityLocationUpdaterRemove {
public:
        template <typename Solution> 
    void operator()(Solution & s, 
            const  typename utils::SolToElem<Solution>::type & se,  //SolutionElement 
            Remove<VertexType> r) {

        auto & FLS = s.get();
        typedef typename std::decay<decltype(FLS)>::type::ObjectType FLS_T;
        FLS.invoke(&FLS_T::remFacility, r.get());
    }
};

template <typename VertexType> 
class FacilityLocationUpdaterAdd {
public:
    template <typename Solution> 
    void operator()(Solution & s, 
            const  typename utils::SolToElem<Solution>::type & se,  //SolutionElement 
            Add<VertexType> a) {

        auto & FLS = s.get();
        typedef typename std::decay<decltype(FLS)>::type::ObjectType FLS_T;
        FLS.invoke(&FLS_T::addFacility, a.get());
    }
};

template <typename VertexType> 
class FacilityLocationUpdaterSwap {
public:
    template <typename Solution> 
    void operator()(Solution & sol, 
            const  typename utils::SolToElem<Solution>::type & se,  //SolutionElement 
            const Swap<VertexType> & s) {
        auto & FLS = sol.get();
        typedef typename std::decay<decltype(FLS)>::type::ObjectType FLS_T;
        FLS.invoke(&FLS_T::addFacility, s.getTo());
        FLS.invoke(&FLS_T::remFacility, s.getFrom());
    }
};


} // facility_location
} // local_search
} // paal
