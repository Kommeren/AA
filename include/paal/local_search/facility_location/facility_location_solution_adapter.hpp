/**
 * @file facility_location_solution_adapter.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */
#include "facility_location_solution_element.hpp"
#include "paal/helpers/type_functions.hpp"
#include "paal/helpers/object_with_copy.hpp"

#include <boost/iterator/transform_iterator.hpp>
#include <boost/range/join.hpp>

namespace paal {
namespace local_search {
namespace facility_location {


template <typename FacilityLocationSolution> class FacilityLocationSolutionAdapter {
    typedef FacilityLocationSolution FLS;
public:
    typedef typename FacilityLocationSolution::VertexType VertexType;
    typedef decltype(std::declval<FacilityLocationSolution>().getChosenFacilities()) Chosen;
    typedef decltype(std::declval<FacilityLocationSolution>().getUnchosenFacilities()) Unchosen;
private:
    typedef Facility<VertexType> Fac;
    typedef std::function<Fac(VertexType)> TransFunct;
    template <typename Col> 
    struct Traits {
        typedef typename helpers::SolToIter<Col>::type ColIter;
        typedef boost::transform_iterator<TransFunct, ColIter> ColTIter;
        typedef std::pair<ColTIter, ColTIter> ColTIterRange;
    };
    typedef typename Traits<Chosen>::ColTIter ChosenTIter;
    typedef typename Traits<Unchosen>::ColTIter UnchosenTIter;
public:
    typedef ObjectWithCopy<FacilityLocationSolution> FacilityLocationSolutionWithCopy;;    
    FacilityLocationSolutionAdapter(FacilityLocationSolution sol) : m_sol(std::move(sol)) {}

    typedef boost::joined_range<typename Traits<Chosen>::ColTIterRange, 
                                typename Traits<Unchosen>::ColTIterRange> Range;
    typedef typename boost::range_iterator<Range>::type SolutionIterator;

    Range getRange() const {
        //TODO examine why doesn't work
        //auto const &  ch = m_sol.invoke(&FLS::getChosenFacilities);
        //auto const &  uch = m_sol.invoke(&FLS::getUnchosenFacilities);
        auto const &  ch = m_sol->getChosenFacilities();
        auto const &  uch = m_sol->getUnchosenFacilities();
        
        auto chb = ch.begin();
        auto che = ch.end();
        auto uchb = uch.begin();
        auto uche = uch.end();

        TransFunct transformChosen = [](VertexType f){ return Fac(CHOSEN ,f);};
        TransFunct transformUnchosen = [](VertexType f){ return Fac(UNCHOSEN, f);};
        
        ChosenTIter chBegin(chb, transformChosen);
        ChosenTIter chEnd(che, transformChosen);

        UnchosenTIter uchBegin(uchb, transformUnchosen);
        UnchosenTIter uchEnd(uche, transformUnchosen);

        typename Traits<Chosen>::ColTIterRange   rch(chBegin, chEnd);
        typename Traits<Unchosen>::ColTIterRange ruch(uchBegin, uchEnd);

        return boost::join(rch, ruch);
    }
    
     SolutionIterator begin() const {
        return boost::begin(getRange());
    }
    
    SolutionIterator end() const {
        return boost::end(getRange());
    }

    FacilityLocationSolutionWithCopy & get() {
        return m_sol;
    }
    
    const FacilityLocationSolutionWithCopy & get() const {
        return m_sol;
    }
    
private:
    FacilityLocationSolutionWithCopy m_sol;    
};

} //facility_location
} // local_search
} // paal
