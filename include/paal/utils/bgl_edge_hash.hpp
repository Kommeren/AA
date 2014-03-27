/**
 * @file bgl_edge_hash.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-12-19
 */
#ifndef BGL_EDGE_HASH_HPP
#define BGL_EDGE_HASH_HPP

#include <boost/functional/hash.hpp>

namespace paal {

/**
 * @brief hash for edge_descriptor in bgl, undirected version
 *
 * @tparam Graph
 * @tparam Enable
 */
template <typename Graph, class Enable = void>
struct edge_hash {
    typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;
    /**
     * @brief operator()
     *
     * @param e
     *
     * @return
     */
    std::size_t operator()(const Edge& e) const {
        std::size_t hash = 0;
        boost::hash_combine(hash, std::min(e.m_source, e.m_target));
        boost::hash_combine(hash, std::max(e.m_source, e.m_target));
        return hash;
    }
};
/**
 * @brief hash for edge_descriptor in bgl, directed version
 *
 * @tparam Graph
 * @tparam Enable
 */

template <typename Graph>
struct edge_hash<Graph, typename std::enable_if<std::is_same<typename boost::graph_traits<Graph>::directed_category, boost::directed_tag>::value>::type>
{
    typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;
    /**
     * @brief operator()
     *
     * @param e
     *
     * @return
     */
    std::size_t operator()(const Edge& e) const {
        std::size_t hash = 0;
        boost::hash_combine(hash, e.m_source);
        boost::hash_combine(hash, e.m_target);
        return hash;
    }
};

}
#endif /* BGL_EDGE_HASH_HPP */
