/**
 * @file facility_location_solution_adapter.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */
#include <unordered_map>
#include <unordered_set>

#include <boost/iterator/transform_iterator.hpp>
#include <boost/range/join.hpp>

#include "paal/utils/type_functions.hpp"
#include "paal/data_structures/object_with_copy.hpp"
#include "paal/data_structures/facility_location/facility_location_solution_traits.hpp"
#include "paal/local_search/facility_location/facility_location_solution_element.hpp"

namespace paal {
namespace local_search {
namespace facility_location {


template <typename FacilityLocationSolution> 
class FacilityLocationSolutionAdapter {
    typedef FacilityLocationSolution FLS;
public:
    typedef typename FacilityLocationSolution::VertexType VertexType;
    typedef Facility<VertexType> Fac;
    typedef std::vector<Fac> Facilities;
    typedef typename Facilities::iterator FacIter;
    typedef decltype(std::declval<FLS>().getChosenFacilities()) Chosen;
    typedef decltype(std::declval<FLS>().getUnchosenFacilities()) Unchosen;
    typedef typename data_structures::FacilityLocationSolutionTraits<FLS>::Dist Dist;
    typedef std::unordered_set<VertexType> UnchosenCopy;
private:
    typedef std::function<Fac(VertexType)> TransFunct;
    template <typename Col> 
    struct Traits {
        typedef typename utils::SolToIter<Col>::type ColIter;
        typedef boost::transform_iterator<TransFunct, ColIter> ColTIter;
        typedef std::pair<ColTIter, ColTIter> ColTIterRange;
    };
    typedef typename Traits<Chosen>::ColTIter ChosenTIter;
    typedef typename Traits<Unchosen>::ColTIter UnchosenTIter;
public:
    typedef data_structures::ObjectWithCopy<FacilityLocationSolution> FacilityLocationSolutionWithCopy;    
    typedef boost::joined_range<typename Traits<Chosen>::ColTIterRange, 
                                typename Traits<Unchosen>::ColTIterRange> Range;
    typedef typename boost::range_iterator<Range>::type SolutionIterator;

    FacilityLocationSolutionAdapter(FacilityLocationSolution sol) : 
            m_sol(std::move(sol)), 
            m_unchosenCopy(m_sol.getUnchosenFacilities().begin(), 
                         m_sol.getUnchosenFacilities().end()) {
        //TODO examine why doesn't work
        //auto const &  ch = m_sol.invoke(&FLS::getChosenFacilities);
        //auto const &  uch = m_sol.invoke(&FLS::getUnchosenFacilities);
        
        auto const &  ch = m_sol.getChosenFacilities();
        auto const &  uch = m_sol.getUnchosenFacilities();
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

        auto join = boost::join(rch, ruch);
        m_facilities.resize(std::distance(boost::begin(join), boost::end(join)));
        std::copy(join.begin(), join.end(), m_facilities.begin());
        for(auto & f : m_facilities) {
            m_vertexToFac.insert(std::make_pair(f.getElem(), &f));
        }
    }

    Dist addFacilityTentative(VertexType v) {
        return m_sol.addFacility(v);
    }
    
    Dist addFacility(Fac & se) {
        auto ret = addFacilityTentative(se.getElem());
        assert(se.getIsChosen() == UNCHOSEN);
        se.setIsChosen(CHOSEN);
        m_unchosenCopy.erase(se.getElem());
        return ret;
    }
    
    Dist removeFacilityTentative(VertexType v) {
        return m_sol.remFacility(v);
    }

    Dist removeFacility(Fac & se) {
        auto ret = removeFacilityTentative(se.getElem());
        assert(se.getIsChosen() == CHOSEN);
        se.setIsChosen(UNCHOSEN);
        m_unchosenCopy.insert(se.getElem());
        return ret;
    }

    Fac & getFacility(VertexType v) {
        auto i = m_vertexToFac.find(v);
        assert(i != m_vertexToFac.end());
        return *(i->second);
    }

    
    FacIter begin() {
        return m_facilities.begin();
    }
    
    FacIter end() {
        return m_facilities.end();
    }

    FacilityLocationSolution & get() {
        return m_sol;
    }
    
    const FacilityLocationSolution & get() const {
        return m_sol;
    }

    const UnchosenCopy & getUnchosenCopy() const {
        return m_unchosenCopy;
    }
    
private:
    FacilityLocationSolution m_sol;    
    Facilities m_facilities;
    std::unordered_map<VertexType, Fac*> m_vertexToFac;
    UnchosenCopy m_unchosenCopy;
};


} //facility_location
} // local_search

namespace data_structures {
    template <typename FacilityLocationSolution>
    struct FacilityLocationSolutionTraits<ObjectWithCopy<FacilityLocationSolution>> : 
                public FacilityLocationSolutionTraits<FacilityLocationSolution> {};
}



} // paal
