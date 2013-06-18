/**
 * @file edmonds_karp_no_init.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-06-17
 */
#ifndef EDMONDS_KARP_NO_INIT_HPP
#define EDMONDS_KARP_NO_INIT_HPP 

namespace boost {

template <typename Graph> 
void edmonds_karp_no_init(Graph &g, 
        typename graph_traits<Graph>::vertex_descriptor source,
        typename graph_traits<Graph>::vertex_descriptor sink) {
    typedef typename graph_traits < Graph>::vertex_descriptor VD;
    typedef typename graph_traits < Graph>::edge_descriptor ED;
    unsigned N = num_vertices(g);
    std::vector<default_color_type> color(N);
    typedef typename property_traits<decltype(&color[0])>::value_type ColorValue;
    typedef color_traits<ColorValue> Color;
    std::vector<ED> pred(N);
    auto rev = get(edge_reverse, g);
    auto res = get(edge_residual_capacity, g);
    put(&color[0], sink, Color::gray());
    while (get(&color[0], sink) != Color::white()) {
      boost::queue<VD> Q;
      breadth_first_search
        (detail::residual_graph(g, res), source, Q,
         make_bfs_visitor(record_edge_predecessors(&pred[0], on_tree_edge())),
         &color[0]);
      if (get(&color[0], sink) != Color::white())
        detail::augment(g, source, sink, &pred[0], res, rev);
    } // while
}

}//boost
#endif /* EDMONDS_KARP_NO_INIT_HPP */
