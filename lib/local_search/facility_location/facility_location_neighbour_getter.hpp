#include "facility_location_update_element.hpp"
#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/range/detail/any_iterator.hpp>

template <typename VertexType> class FacilityLocationNeighbourGetter {
public: 
        
    typedef boost::range_detail::any_iterator<UpdateElement, boost::forward_traversal_tag, const UpdateElement &, std::ptrdiff_t> Iter;


    typedef SolutionElement<VertexType> SolEl;
    template <typename Solution> std::pair<Iter, Iter>
        getNeighbourhood(Solution &s, SolEl & el) {

        auto FCS =  s.getFacilityLocationSolution(); 
        m_currSol.clear();
        if(el.getIsChosen() == UNCHOSEN) {
            m_rem = Remove<VertexType>(el.getElem());
            m_currSol.push_back(UpdateElement(&m_rem));
//            auto r = boost::join 
        }
    }
private:
    Remove<VertexType> m_rem;
    Add<VertexType> m_add;
    std::vector<UpdateElement> m_currSol;
};
