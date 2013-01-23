#include "facility_location_solution_element.hpp"

#include <boost/iterator/transform_iterator.hpp>
#include <boost/range/join.hpp>

template <typename FacilityLocationSolution> class FacilityLocationSolutionAdapter {
public:
    FacilityLocationSolutionAdapter(FacilityLocationSolution & sol) : m_sol(sol) {}

    typedef typename FacilityLocationSolution::VertexType VertexType;
    typedef SolutionElement<VertexType> SolElemT;
    typedef decltype(std::declval<FacilityLocationSolution>().getChosenFacilities().begin()) ChosenIter;
    typedef decltype(std::declval<FacilityLocationSolution>().getUnchosenFacilities().begin()) UnchosenIter;
    typedef std::function<SolElemT(VertexType)> TransFunct;
    typedef boost::transform_iterator<TransFunct, ChosenIter> ChosenTIter;
    typedef boost::transform_iterator<TransFunct, UnchosenIter> UnchosenTIter;
    typedef std::pair<ChosenTIter, ChosenTIter> ChosenTIterRange;
    typedef std::pair<UnchosenTIter, UnchosenTIter> UnchosenTIterRange;
    typedef boost::joined_range<ChosenTIterRange, UnchosenTIterRange> Range;
    typedef typename boost::range_iterator<Range>::type SolutionIterator;

    Range getRange(ChosenIter chb, ChosenIter che, UnchosenIter uchb, UnchosenIter uche) const {
        auto ch = m_sol.getChosenFacilities();
        auto uch = m_sol.getUnchosenFacilities();

        TransFunct transformChosen = [](VertexType v){ return SolElemT(CHOSEN ,v);};
        TransFunct transformUnchosen = [](VertexType v){ return SolElemT(UNCHOSEN, v);};
        
        ChosenTIter chBegin(chb);
        ChosenTIter chEnd(che);

        UnchosenTIter uchBegin(uchb);
        UnchosenTIter uchEnd(uche);

        ChosenTIterRange rch(chBegin, chEnd);
        UnchosenTIterRange ruch(uchBegin, uchEnd);

        return boost::join(rch, ruch);
    }
    
     SolutionIterator cbegin() const {
        auto & ch = m_sol.getChosenFacilities();
        auto & uch = m_sol.getUnchosenFacilities();
        return boost::begin(getRange(ch.begin(), ch.end(), uch.begin(), uch.end()));
    }
    
    SolutionIterator cend() const {
        auto & ch = m_sol.getChosenFacilities();
        auto & uch = m_sol.getUnchosenFacilities();
        return boost::end(getRange(ch.begin(), ch.end(), uch.begin(), uch.end()));
    }

    FacilityLocationSolution & getFacilityLocationSolution () {
        return m_sol;
    }
private:
    FacilityLocationSolution & m_sol;    
};

