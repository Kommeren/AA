#include "facility_location_solution_element.hpp"
#include "helpers/type_functions.hpp"

#include <boost/iterator/transform_iterator.hpp>
#include <boost/range/join.hpp>

namespace paal {
namespace local_search {
namespace facility_location {

template <typename FacilityLocationSolution> class FacilityLocationSolutionAdapter {
public:
    FacilityLocationSolutionAdapter(FacilityLocationSolution & sol) : m_sol(sol) {}

    typedef typename FacilityLocationSolution::VertexType VertexType;
    typedef SolutionElement<VertexType> SolElemT;
    typedef decltype(std::declval<FacilityLocationSolution>().getChosenFacilities()) ChosenType;
    typedef decltype(std::declval<FacilityLocationSolution>().getUnchosenFacilities()) UnchosenType;
    typedef typename SolToIter<ChosenType>::type ChosenIter;
    typedef typename SolToIter<UnchosenType>::type UnchosenIter;
    typedef std::function<SolElemT(VertexType)> TransFunct;
    typedef boost::transform_iterator<TransFunct, ChosenIter> ChosenTIter;
    typedef boost::transform_iterator<TransFunct, UnchosenIter> UnchosenTIter;
    typedef std::pair<ChosenTIter, ChosenTIter> ChosenTIterRange;
    typedef std::pair<UnchosenTIter, UnchosenTIter> UnchosenTIterRange;
    typedef boost::joined_range<ChosenTIterRange, UnchosenTIterRange> Range;
    typedef typename boost::range_iterator<Range>::type SolutionIterator;

    Range getRange() const {
        auto ch = m_sol.getChosenFacilities();
        auto uch = m_sol.getUnchosenFacilities();
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

    FacilityLocationSolution & get() {
        return m_sol;
    }
private:
    FacilityLocationSolution & m_sol;    
};

} //facility_location
} // local_search
} // paal



