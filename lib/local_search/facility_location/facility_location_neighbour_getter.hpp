#include "facility_location_update_element.hpp"
#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/range/detail/any_iterator.hpp>

namespace paal {
namespace local_search {
namespace facility_location {

template <typename VertexType> class U {
public:
    U(VertexType v) : m_u(&m_sw), m_from(v) {}
    U() : m_u(&m_sw) {}

    const Update & operator()(VertexType v) const {
        m_sw.setFrom(m_from); 
        m_sw.setTo(v);
        return m_u;
    }

private:
    mutable Swap<VertexType> m_sw;
    Update m_u;
    VertexType m_from;
};


template <typename VertexType> class FacilityLocationNeighbourhoodGetter {
public: 
       
    //TODO we shouldn't use detail..
    typedef boost::range_detail::any_iterator<Update, boost::forward_traversal_tag, const Update &, std::ptrdiff_t> Iter;

    //Due to the memory optimization at one moment only one Update is valid
    typedef SolutionElement<VertexType> SolEl;
    template <typename Solution> std::pair<Iter, Iter>
        get(Solution &s, const SolEl & el) {

        auto & FCS =  s.get(); 
        m_currSol.clear();

        if(el.getIsChosen() == UNCHOSEN) {
            //the update of UNCHOSEN is just adding him to solution
            m_add.set(el.getElem());
            m_currSol.push_back(Update(&m_add));
            return std::make_pair(m_currSol.begin(), m_currSol.end()); 
        } else {
            assert(el.getIsChosen() == CHOSEN); 
            //the update of CHOSEN could be remove or swap with some unchosen
            m_rem.set(el.getElem());
            m_currSol.push_back(Update(&m_rem));

            U<VertexType> uchToUE(el.getElem());

            auto remRange = std::make_pair(m_currSol.begin(), m_currSol.end());
            auto & uch = FCS.getUnchosenFacilities();
            typedef boost::transform_iterator<U<VertexType>, decltype(uch.begin()), const Update &> TransIter;
            auto swapRange = std::make_pair(TransIter(uch.begin(), uchToUE), TransIter(uch.end(), uchToUE)); 

            auto ret = boost::join(remRange, swapRange); 
            
            return std::make_pair(boost::begin(ret), boost::end(ret));
        }
    }
private:
    Remove<VertexType> m_rem;
    Add<VertexType> m_add;
    std::vector<Update> m_currSol;
};

} // facility_location
} // local_search
} // paal
