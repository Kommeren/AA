/**
 * @file facility_location_swap.hpp
* @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-07-08
 */
#ifndef FACILITY_LOCATION_SWAP_HPP
#define FACILITY_LOCATION_SWAP_HPP 
#include <cassert>
#include <vector>
#include <numeric>
#include <cstdlib>

#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/iterator/transform_iterator.hpp>

#include "paal/data_structures/facility_location/facility_location_solution_traits.hpp"
#include "paal/utils/type_functions.hpp"
#include "paal/local_search/facility_location/facility_location_solution_element.hpp"

namespace paal {
namespace local_search {
namespace facility_location {

template <typename T> class Swap {
public:
    Swap(T from, T to) : m_from(from), m_to(to) {}
    Swap() {}

    T getFrom() const {
        return m_from;
    }

    T getTo() const {
        return m_to;
    }
    
    void setFrom(T from) {
        m_from = from;
    }

    void setTo(T to) {
        m_to = to;
    }

private:
    T m_from;
    T m_to;
};

template <typename VertexType> class VertexToSwapMove {
public:
    VertexToSwapMove(VertexType v) : m_from(v) {}
    
    VertexToSwapMove() = default;
    VertexToSwapMove(const VertexToSwapMove & u) = default;
    
    VertexToSwapMove & operator=(const VertexToSwapMove & u)  { 
        m_from = u.m_from;
        return *this;
    }

    const Swap<VertexType> & operator()(VertexType v) const {
        m_sw.setFrom(m_from); 
        m_sw.setTo(v);
        return m_sw;
    }

private:
    mutable Swap<VertexType> m_sw;
    VertexType m_from;
};

template <typename VertexType> 
class FacilityLocationCheckerSwap {
public:
        template <class Solution> 
    auto operator()(Solution & sol, 
            const  typename utils::CollectionToElem<Solution>::type & se,  //SolutionElement 
            const Swap<VertexType> & s) ->
                typename data_structures::FacilityLocationSolutionTraits<puretype(sol.get())>::Dist {
        typename data_structures::FacilityLocationSolutionTraits<puretype(sol.get())>::Dist ret, back;
        
        ret   = sol.addFacilityTentative(s.getTo());
        ret  += sol.removeFacilityTentative(s.getFrom());
        back  = sol.addFacilityTentative(s.getFrom());
        back += sol.removeFacilityTentative(s.getTo());
        assert(ret == -back);
        return -ret;
    }
};


template <typename VertexType> 
class FacilityLocationCommitSwap {
public:
    template <typename Solution> 
    void operator()(Solution & sol, 
            const  typename utils::CollectionToElem<Solution>::type & se,  //SolutionElement 
            const Swap<VertexType> & s) {
        sol.addFacility(sol.getFacility(s.getTo()));
        sol.removeFacility(sol.getFacility(s.getFrom()));
    }
};

template <typename VertexType> 
class FacilityLocationGetMovesSwap {
    template <typename Solution>
    struct IterType {
        typedef puretype(std::declval<const Solution &>().getUnchosenCopy()) Unchosen; 
        typedef typename utils::CollectionToIter<const Unchosen>::type UchIter; 
        typedef boost::transform_iterator<VertexToSwapMove<VertexType>, 
                 UchIter, const Swap<VertexType> &> TransIter;
        typedef std::pair<TransIter, TransIter> TransRange;
    };

public: 
    typedef Facility<VertexType> Fac;

    //Due to the memory optimization at one moment only one Move is valid
    template <typename Solution> typename IterType<Solution>::TransRange
    operator()(const Solution &s, const Fac & el) {
        auto e = el.getElem();

        typedef typename IterType<Solution>::TransIter TransIter;

        if(el.getIsChosen() == CHOSEN) {
            //the move of CHOSEN could be swap with some unchosen
            auto const & uch = s.getUnchosenCopy();
            VertexToSwapMove<VertexType> uchToUE(e);
            return std::make_pair(TransIter(uch.begin(), uchToUE), 
                                  TransIter(uch.end()  , uchToUE)); 
        }
        return std::make_pair(TransIter(), TransIter()); 
    }
};

} // facility_location
} // local_search
} // paal

#endif /* FACILITY_LOCATION_SWAP_HPP */
