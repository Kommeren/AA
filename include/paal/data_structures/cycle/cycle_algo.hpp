/**
 * @file cycle_algo.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */
#include <algorithm>
#include "paal/data_structures/cycle/cycle_traits.hpp"
#include "paal/helpers/vertex_to_edge_iterator.hpp"

namespace paal {
namespace simple_algo {


template <typename Metric, typename Cycle> typename Metric::DistanceType getLength(const Metric & m, const Cycle & cm) {
    typedef typename data_structures::CycleTraits<Cycle>::CycleElem El;
    typedef typename Metric::DistanceType Dist;
    
    auto ebegin = helpers::make_VertexToEdgeIterator(cm.vbegin(), cm.vend());
    auto eend = helpers::make_VertexToEdgeIterator(cm.vend(), cm.vend());
    return std::accumulate(ebegin, eend, Dist(), [&m]
            (Dist a, const std::pair<El, El> & p)->Dist { 
                return a + m(p.first, p.second);
                }
            );    
}


template <typename Cycle, typename Stream> void print(const Cycle & cm, Stream & o, const  std::string & endl = "\n") {
    auto ebegin = helpers::make_VertexToEdgeIterator(cm.vbegin(), cm.vend());
    auto eend = helpers::make_VertexToEdgeIterator(cm.vend(), cm.vend());
    typedef typename data_structures::CycleTraits<Cycle>::CycleElem El;

    for(const std::pair<El, El> & p : helpers::make_range(ebegin, eend)){ 
        o <<  "(" << p.first << "," << p.second << ")->";
    }
        
    o << endl;
}

}//paal
}//simple_algo
