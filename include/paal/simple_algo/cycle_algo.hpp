/**
 * @file cycle_algo.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */
#include <algorithm>

namespace paal {
namespace simple_algo {


template <typename Metric, typename CycleManager> typename Metric::DistanceType getLength(const Metric & m, const CycleManager & cm) {
    typedef typename CycleManager::CycleElement El;
    typedef typename Metric::DistanceType Dist;
    
    auto edges = cm.getEdgeRange();
    return std::accumulate(edges.first, edges.second, Dist(), [&m]
            (Dist a, const std::pair<El, El> & p)->Dist { 
                return a + m(p.first, p.second);
                }
            );    
}


template <typename CycleManager, typename Stream> void print(const CycleManager & cm, Stream & o, const  std::string & endl = "\n") {
    auto edges = cm.getEdgeRange();
    typedef typename CycleManager::CycleElement El;

    for(const std::pair<El, El> & p : helpers::make_range(edges)){ 
        o <<  "(" << p.first << "," << p.second << ")->";
    }
        
    o << endl;
}

}//paal
}//simple_algo
