/**
 * @file facility_location_add.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-07-08
 */
#ifndef FACILITY_LOCATION_ADD_HPP
#define FACILITY_LOCATION_ADD_HPP 

#include <vector>
#include <numeric>
#include <cstdlib>
#include <cassert>

#include <boost/iterator/iterator_adaptor.hpp>

#include "paal/utils/type_functions.hpp"
#include "paal/data_structures/facility_location/facility_location_solution_traits.hpp"
#include "paal/utils/iterator_utils.hpp"
#include "paal/utils/type_functions.hpp"
#include "paal/local_search/facility_location/facility_location_solution_element.hpp"


namespace paal {
namespace local_search {
namespace facility_location {

template <typename T> class Add {
public:
    Add(T t) : m_t(t) {}
    Add() {}

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
class FacilityLocationGetNeighborhoodAdd {
    typedef Add<VertexType> AddType;
    typedef std::vector<AddType> Updates;
    typedef typename Updates::iterator Iter;

public: 
    typedef Facility<VertexType> Fac;

    template <typename Solution> 
        std::pair<Iter, Iter>
    operator()(const Solution &, const Fac & el) {
        auto e = el.getElem();

        m_currSol.clear();
        if(el.getIsChosen() == UNCHOSEN) {
            //the update of UNCHOSEN could be added to the solution
            m_currSol.push_back(e);
        }
        return std::make_pair(m_currSol.begin(), m_currSol.end());
    }
private:
    Updates m_currSol;
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


} // facility_location
} // local_search
} // paal

#endif /* FACILITY_LOCATION_ADD_HPP */
