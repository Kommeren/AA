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


template <typename T> 
class Remove {
public:
    Remove(T t) : m_t(t) {}
    Remove() {}

    T get() const {
        return m_t;
    }
    
    void set(T t) {
        m_t = t;
    }

private:
    T m_t;
};

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
class FacilityLocationGetNeighborhoodRemove {
    typedef Remove<VertexType> RemoveType;
    typedef std::vector<RemoveType> Updates;
    typedef typename Updates::iterator Iter;

public: 
    typedef Facility<VertexType> Fac;

    template <typename Solution> 
        typename std::pair<Iter, Iter>
    operator()(const Solution &, const Fac & el) {
        auto e = el.getElem();
         
        m_currSol.clear();
        if(el.getIsChosen() == CHOSEN) { 
            //the update of CHOSEN could be remove
            m_currSol.push_back(e);
        }
        return std::make_pair(m_currSol.begin(), m_currSol.end());
    }
private:
    Updates m_currSol;
};


} // facility_location
} // local_search
} // paal

#endif /* FACILITY_LOCATION_REMOVE_HPP */
