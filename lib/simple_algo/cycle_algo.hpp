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

    std::for_each(edges.first, edges.second, [&]
            (const std::pair<El, El> & p)->void { 
                    o <<  "(" << p.first << "," << p.second << ")->";
                }
            );    
    o << endl;
}

}//paal
}//simple_algo