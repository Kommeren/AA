/**
 * @file fl_algo.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-15
 */
#ifndef FL_ALGO_HPP
#define FL_ALGO_HPP 
namespace paal {
namespace simple_algo {


    template <typename Metric, typename FCosts, typename FLSolution> 
       typename Metric::DistanceType
getFLCost(const Metric & m, const FCosts & fcosts, const FLSolution & fls) {
    auto const & voronoi = fls.getVoronoi();
    auto const & ch      = voronoi.getGenerators();

    typedef typename std::decay<decltype(voronoi)>::type Voronoi;
    typedef typename Voronoi::Dist Dist;
    typedef typename Voronoi::VertexType VertexType;

    Dist d = std::accumulate(ch.begin(), ch.end(), double(0.), [&](Dist d, VertexType f){return d+fcosts(f);});

    for(VertexType f : ch) {
        for(VertexType v : voronoi.getVerticesForGenerator(f)) {
            d += m(v, f); 
        }
    }

    return d;
}

}//paal
}//simple_algo
#endif /* FL_ALGO_HPP */
