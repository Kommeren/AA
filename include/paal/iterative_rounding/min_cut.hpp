/**
 * @file min_cut.hpp
 * @brief
 * @author Piotr Godlewski
 * @version 1.0
 * @date 2014-03-05
 */
#ifndef MIN_CUT_HPP
#define MIN_CUT_HPP

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/boykov_kolmogorov_max_flow.hpp>

namespace paal {
namespace ir {

/**
 * @class MinCutFinder
 * @brief Class for creating and modifying directed graphs with edge capacities
 *  and finding directed minimum cuts between given vertices.
 */
class MinCutFinder {
    typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> Traits;
public:
    typedef Traits::edge_descriptor Edge;
    typedef Traits::vertex_descriptor Vertex;

    /// Constructor.
    MinCutFinder() :
        m_graph(0),
        m_cap(get(boost::edge_capacity, m_graph)),
        m_rev(get(boost::edge_reverse, m_graph)),
        m_colors(get(boost::vertex_color, m_graph))
    { }

    /**
     * (Re)Initializes the graph.
     */
    void init(int verticesNum) {
        m_graph.clear();
        for (int i = 0; i < verticesNum; ++i) {
            addVertex();
        }
        m_cap = get(boost::edge_capacity, m_graph);
        m_rev = get(boost::edge_reverse, m_graph);
    }

    /**
     * Adds a new vertex to the graph.
     */
    Vertex addVertex() {
        return add_vertex(m_graph);
    }

    /**
     * Adds an edge to the graph.
     * @param src source vertex of for the added edge
     * @param trg target vertex of for the added edge
     * @param cap capacity of the added edge
     * @param revCap capacity of the reverse edge
     *
     * @return created edge of the graph and the created reverse edge
     */
    std::pair<Edge, Edge>
    addEdge(Vertex src, Vertex trg, double cap, double revCap = 0.) {
        bool b, bRev;
        Edge e, eRev;

        std::tie(e, b) = add_edge(src, trg, m_graph);
        std::tie(eRev, bRev) = add_edge(trg, src, m_graph);
        assert(b && bRev);

        cap = std::max(0., cap);
        revCap = std::max(0., revCap);
        put(m_cap, e, cap);
        put(m_cap, eRev, revCap);

        put(m_rev, e, eRev);
        put(m_rev, eRev, e);

        return std::make_pair(e, eRev);
    }

    /**
     * Finds the min cut between \c src and \c trg.
     *
     * @param src source vertex (belongs to the cut set)
     * @param trg target vertex (does not belong to the cut set)
     *
     * @return min cut value
     */
    double findMinCut(Vertex src, Vertex trg) {
        assert(src != trg);
        double minCutVal = boost::boykov_kolmogorov_max_flow(m_graph, src, trg);
        m_colors = get(boost::vertex_color, m_graph);
        m_srcColor = get(m_colors, src);
        m_lastCut = std::make_pair(src, trg);
        assert(!isInSourceSet(trg));
        return minCutVal;
    }

    /**
     * Checks if the given vertex belongs to the source side of the last checked cut.
     */
    bool isInSourceSet(Vertex v) const {
        return (m_srcColor == get(m_colors, v));
    }

    /**
     * Returns the number of vertices in the source size of the last checked cut.
     */
    int sourceSetSize() const {
        auto verts = vertices(m_graph);
        return std::accumulate(verts.first, verts.second, 0,
                [&](int count, Vertex v){ return count + isInSourceSet(v); });
    }


    /**
     * Returns the pair of vertices defining the last checked cut.
     */
    std::pair<Vertex, Vertex> getLastCut() const {
        return m_lastCut;
    }

    /**
     * Returns the capacity of a given edge.
     */
    double getCapacity(Edge e) const {
        return get(m_cap, e);
    }

    /**
     * Sets the capacity of a given edge.
     */
    void setCapacity(Edge e, double cap) {
        put(m_cap, e, cap);
    }

private:
    typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS,
                                  boost::property<boost::vertex_color_t, boost::default_color_type,
                                      boost::property<boost::vertex_distance_t, long,
                                          boost::property<boost::vertex_predecessor_t, Edge>>>,
                                  boost::property<boost::edge_capacity_t, double,
                                      boost::property<boost::edge_residual_capacity_t, double,
                                          boost::property<boost::edge_reverse_t, Edge>>>
                                 > Graph;
    typedef boost::property_map<Graph, boost::edge_capacity_t>::type EdgeCapacity;
    typedef boost::property_map<Graph, boost::edge_reverse_t>::type  EdgeReverse;
    typedef boost::property_map<Graph, boost::vertex_color_t>::type  VertexColors;

    Graph m_graph;

    EdgeCapacity m_cap;
    EdgeReverse  m_rev;
    VertexColors m_colors;

    boost::default_color_type m_srcColor;
    std::pair<Vertex, Vertex> m_lastCut;
};


} //ir
} //paal
#endif /* MIN_CUT_HPP */
