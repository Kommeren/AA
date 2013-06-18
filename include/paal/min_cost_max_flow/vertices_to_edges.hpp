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
class MapVerticesToEdges : 
    public put_get_helper<typename graph_traits<Graph>::edge_descriptor, MapVerticesToEdges<Pred, Graph>> {
    typedef graph_traits<Graph> gtraits;
public:
    typedef boost::readable_property_map_tag category;
    typedef typename gtraits::edge_descriptor value_type;
    typedef value_type reference;
    typedef typename gtraits::vertex_descriptor key_type;
    MapVerticesToEdges(Pred p, const Graph & g) : 
        pred_(p), g_(g) {}

     reference operator[](key_type v) const {
        bool b;
        value_type e;
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

} // boost
#endif /* VERTICES_TO_EDGES_HPP */
