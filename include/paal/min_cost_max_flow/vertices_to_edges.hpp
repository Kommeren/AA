/**
 * @file vertices_to_edges.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-06-14
 */
#ifndef VERTICES_TO_EDGES_HPP
#define VERTICES_TO_EDGES_HPP 

namespace boost {
namespace detail {
template <typename Pred, typename Graph>
class MapVerticesToEdges {
    typedef graph_traits<Graph> gtraits;
    typedef typename gtraits::edge_descriptor ED;
    typedef typename gtraits::vertex_descriptor VD;
public:
    MapVerticesToEdges(Pred p, const Graph & g) : 
        pred_(p), g_(g) {}

     ED operator()(VD v) const {
        bool b;
        ED e;
        tie(e ,b) = edge(get(pred_, v), v, g_);
//        assert(b);
        return e;
    }
private:
    Pred pred_;
    const Graph & g_;
};

template <typename Pred, typename Graph>
MapVerticesToEdges<Pred, Graph> 
make_mapVerticesToEdges(const Pred & p, const Graph & g)  {
    return MapVerticesToEdges<Pred, Graph>(p, g);
}

}//detail

template <typename Pred, typename Graph>
typename graph_traits<Graph>::edge_descriptor 
get(const detail::MapVerticesToEdges<Pred, Graph> & map, typename graph_traits<Graph>::vertex_descriptor v) {
    return map(v);
}

} // boost
#endif /* VERTICES_TO_EDGES_HPP */
