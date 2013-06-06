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

#include "paal/utils/iterator_utils.hpp"
#include "paal/utils/type_functions.hpp"

#include "vertex_to_swap_update.hpp"
#include "facility_location_solution_element.hpp"

namespace paal {
namespace local_search {
namespace facility_location {



template <typename VertexType> 
class FacilityLocationGetNeighborhood {
    typedef std::vector<Update> Updates;
    template <typename Solution>
    struct IterType {
        typedef puretype(std::declval<Solution>().get()) InnerSol; 
        typedef puretype(std::declval<InnerSol>()->getUnchosenFacilities()) Unchosen; 
        typedef typename utils::SolToIter<Unchosen>::type UchIter; 
        typedef boost::transform_iterator<VertexToSwapUpdate<VertexType>, 
                 UchIter, const Update &> TransIter;
        typedef std::pair<TransIter, TransIter> TransRange;

        typedef Updates::iterator CurrIter;
        typedef std::pair<CurrIter, CurrIter> CurrRange;

        typedef boost::joined_range<CurrRange, TransRange> Join;
        typedef decltype(std::make_pair(begin(std::declval<Join>()), end(std::declval<Join>()))) Ret;
    };

public: 
    typedef Facility<VertexType> Fac;

    //Due to the memory optimization at one moment only one Update is valid
    template <typename Solution> typename IterType<Solution>::Ret
    operator()(const Solution &s, const Fac & el) {
        typedef IterType<Solution> Traits;

        auto const & FCS = s.get(); 
        m_currSol.clear();
        auto e = el.getElem();

        if(el.getIsChosen() == UNCHOSEN) {
            //the update of UNCHOSEN is just adding him to solution
            m_add.set(el.getElem());
            m_currSol.push_back(Update(&m_add));
            typename Traits::CurrRange res = std::make_pair(m_currSol.begin(), m_currSol.end()); 
            typedef IterType<Solution> Traits;
            typename Traits::TransRange empty;
            auto join =  boost::join(res, empty);
            return std::make_pair(begin(join), end(join));
        } else {
            assert(el.getIsChosen() == CHOSEN); 
    
            //the update of CHOSEN could be remove or swap with some unchosen
            m_rem.set(e);
            m_currSol.push_back(Update(&m_rem));

            VertexToSwapUpdate<VertexType> uchToUE(e);

            auto remRange = std::make_pair(m_currSol.begin(), m_currSol.end());
            auto const & uch = FCS->getUnchosenFacilities();

            typedef boost::transform_iterator<VertexToSwapUpdate<VertexType>, 
                 decltype(uch.begin()), const Update &> TransIter;

            auto swapRange = std::make_pair(TransIter(uch.begin(), uchToUE), 
                                        TransIter(uch.end()  , uchToUE)); 

            auto ret = boost::join(remRange, swapRange); 
        
            return std::make_pair(boost::begin(ret), boost::end(ret));
        }
    }
private:
    Remove<VertexType> m_rem;
    Add<VertexType> m_add;
    Updates m_currSol;
};

} // facility_location
} // local_search
} // paal
