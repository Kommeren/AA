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
    typedef ObjectWithCopy<FacilityLocationSolution> FacilityLocationSolutionWithCopy;;    
    FacilityLocationSolutionAdapter(FacilityLocationSolution sol) : m_sol(std::move(sol)) {}

    typedef typename FacilityLocationSolution::VertexType VertexType;
    typedef SolutionElement<VertexType> SolElemT;
    typedef decltype(std::declval<FacilityLocationSolution>().getChosenFacilities()) ChosenType;
    typedef decltype(std::declval<FacilityLocationSolution>().getUnchosenFacilities()) UnchosenType;
    typedef typename helpers::SolToIter<ChosenType>::type ChosenIter;
    typedef typename helpers::SolToIter<UnchosenType>::type UnchosenIter;
    typedef std::function<SolElemT(VertexType)> TransFunct;
    typedef boost::transform_iterator<TransFunct, ChosenIter> ChosenTIter;
    typedef boost::transform_iterator<TransFunct, UnchosenIter> UnchosenTIter;
    typedef std::pair<ChosenTIter, ChosenTIter> ChosenTIterRange;
    typedef std::pair<UnchosenTIter, UnchosenTIter> UnchosenTIterRange;
    typedef boost::joined_range<ChosenTIterRange, UnchosenTIterRange> Range;
    typedef typename boost::range_iterator<Range>::type SolutionIterator;

    Range getRange() const {
        //TODO examine why doesn't work
        //auto const &  ch = m_sol.invoke(&FLS::getChosenFacilities);
        //auto const &  uch = m_sol.invoke(&FLS::getUnchosenFacilities);
        auto const &  ch = m_sol->getChosenFacilities();
        auto const &  uch = m_sol->getUnchosenFacilities();
        ChosenIter chb = ch.begin();
        ChosenIter che = ch.end();
        UnchosenIter uchb = uch.begin();
        UnchosenIter uche = uch.end();

        TransFunct transformChosen = [](VertexType v){ return SolElemT(CHOSEN ,v);};
        TransFunct transformUnchosen = [](VertexType v){ return SolElemT(UNCHOSEN, v);};
        
        ChosenTIter chBegin(chb, transformChosen);
        ChosenTIter chEnd(che, transformChosen);

        UnchosenTIter uchBegin(uchb, transformUnchosen);
        UnchosenTIter uchEnd(uche, transformUnchosen);

        ChosenTIterRange rch(chBegin, chEnd);
        UnchosenTIterRange ruch(uchBegin, uchEnd);

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



