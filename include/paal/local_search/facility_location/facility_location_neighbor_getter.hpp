/**
 * @file facility_location_neighbor_getter.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */
    
#include <vector>
#include <numeric>
#include <cstdlib>

#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/iterator/transform_iterator.hpp>

#include "paal/utils/iterator_utils.hpp"
#include "paal/utils/type_functions.hpp"

#include "vertex_to_swap_update.hpp"
#include "facility_location_solution_element.hpp"

namespace paal {
namespace local_search {
namespace facility_location {

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
class FacilityLocationGetNeighborhoodSwap {
    template <typename Solution>
    struct IterType {
        typedef puretype(std::declval<Solution>().get()) InnerSol; 
        typedef puretype(std::declval<InnerSol>()->getUnchosenFacilities()) Unchosen; 
        typedef typename utils::SolToIter<Unchosen>::type UchIter; 
        typedef boost::transform_iterator<VertexToSwapUpdate<VertexType>, 
                 UchIter, const Swap<VertexType> &> TransIter;
        typedef std::pair<TransIter, TransIter> TransRange;
    };

public: 
    typedef Facility<VertexType> Fac;

    //Due to the memory optimization at one moment only one Update is valid
    template <typename Solution> typename IterType<Solution>::TransRange
    operator()(const Solution &s, const Fac & el) {
        auto const & FCS = s.get(); 
        auto e = el.getElem();

        auto const & uch = FCS->getUnchosenFacilities();

        typedef boost::transform_iterator<VertexToSwapUpdate<VertexType>, 
             decltype(uch.begin()), const Swap<VertexType> &> TransIter;

        if(el.getIsChosen() == CHOSEN) {
            //the update of CHOSEN could be swap with some unchosen
            VertexToSwapUpdate<VertexType> uchToUE(e);
            return std::make_pair(TransIter(uch.begin(), uchToUE), 
                                        TransIter(uch.end()  , uchToUE)); 
        }
        return std::make_pair(TransIter(), TransIter()); 
    }
};

} // facility_location
} // local_search
} // paal
