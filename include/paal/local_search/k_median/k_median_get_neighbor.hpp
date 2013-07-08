/**
 * @file k_median_get_neighbor.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-03-08
 */
#ifndef K_MEDIAN_GET_NEIGHBOR_HPP
#define K_MEDIAN_GET_NEIGHBOR_HPP 

#include "paal/local_search/facility_location/vertex_to_swap_update.hpp"
#include "paal/local_search/facility_location/facility_location_solution_element.hpp"

namespace paal {
namespace local_search {
namespace k_median {

template <typename VertexType>
class KMedianGetNeighborhood {
public:
    typedef facility_location::Facility<VertexType> Fac;

        template <typename Solution> 
    auto operator()(const Solution &s, const Fac & el) ->
    std::pair<boost::transform_iterator<facility_location::VertexToSwapUpdate<VertexType>, 
                 decltype(s.get()->getUnchosenFacilities().begin()), const facility_location::Swap<VertexType> &>,
              boost::transform_iterator<facility_location::VertexToSwapUpdate<VertexType>,
                 decltype(s.get()->getUnchosenFacilities().begin()), const facility_location::Swap<VertexType> &>>
    {
        typedef boost::transform_iterator<facility_location::VertexToSwapUpdate<VertexType>, 
                  decltype(s.get()->getUnchosenFacilities().begin()), const facility_location::Swap<VertexType> &> NeighIter;

        auto const & FCS = s.get(); 
        auto e = el.getElem();

        if(el.getIsChosen() == facility_location::UNCHOSEN) {
            return std::make_pair(NeighIter(), NeighIter()); 
        } else {
            assert(el.getIsChosen() == facility_location::CHOSEN); 
    
            //the update is allway a swap with some unchosen

            facility_location::VertexToSwapUpdate<VertexType> uchToUE(e);

            auto const & uch = FCS->getUnchosenFacilities();

            return std::make_pair(NeighIter(uch.begin(), uchToUE), 
                              NeighIter(uch.end()  , uchToUE)); 
        }
    }
private:
};

}
}
}

#endif /* K_MEDIAN_GET_NEIGHBOR_HPP */
