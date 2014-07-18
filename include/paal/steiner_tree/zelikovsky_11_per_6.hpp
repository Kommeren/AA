/**
 * @file zelikovsky_11_per_6.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-07-24
 */

// hack for clang compilation (hash for boost tuple, boost::1_55)
#include <boost/config.hpp>
#undef BOOST_NO_CXX11_HDR_TUPLE

#include "paal/utils/contract_bgl_adjacency_matrix.hpp"
#include "paal/data_structures/subset_iterator.hpp"
#include "paal/data_structures/metric/metric_to_bgl.hpp"
#include "paal/data_structures/metric/metric_traits.hpp"
#include "paal/data_structures/voronoi/voronoi.hpp"
#include "paal/data_structures/metric/graph_metrics.hpp"
#include "paal/local_search/local_search.hpp"
#include "paal/utils/functors.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/subgraph.hpp>
#include <boost/graph/prim_minimum_spanning_tree.hpp>

#include <boost/range/join.hpp>
#include <boost/range/irange.hpp>
#include <boost/range/algorithm/copy.hpp>

#include <boost/iterator/zip_iterator.hpp>
#include <boost/iterator/transform_iterator.hpp>

#include <boost/functional/hash.hpp>

#include <unordered_map>
#include <stack>

namespace paal {

namespace detail {
/**
 * class steiner_tree
 * @brief This is Alexander Zelikovsky 11/6 approximation algorithm for steiner
 tree.
 *
 * Example: <br>
   \snippet zelikovsky_11_per_6_example.cpp Steiner Tree Example
 *
 * example file is steiner_tree_example.cpp
 *
 * @tparam Metric we only use this metric for distances  (Steiner, Terminal) and
 (Terminal, Terminal)
 * @tparam voronoi models WeakVronoi (see \ref voronoi). This is a voronoi
 division where generators are terminals  of the steiner tree.
 */
template <typename Metric, typename voronoi> class steiner_tree {
    typedef int Idx;

  public:
    typedef data_structures::metric_traits<Metric> MT;
    typedef typename MT::DistanceType Dist;
    typedef typename MT::VertexType VertexType;
    static const int SUSBSET_SIZE = 3;

    typedef typename utils::k_tuple<Idx, SUSBSET_SIZE>::type ThreeTuple;
    typedef boost::tuple<ThreeTuple, Dist> Move;
    typedef std::vector<VertexType> ResultSteinerVertices;

    /**
     * @brief
     *
     * @param m
     * @param voronoi
     */
    steiner_tree(const Metric &m, const voronoi &vor)
        : m_metric(m), m_voronoi(vor),
          N(std::distance(vor.get_generators().begin(),
                          vor.get_generators().end())),
          m_save(N) {}

    template <typename OutputIterator>
    void get_result_steiner_vertices(OutputIterator out) {
        ResultSteinerVertices res;

        if (m_voronoi.get_vertices().empty()) {
            return;
        }

        auto ti = boost::irange<int>(0, N);
        auto subsets = this->make_three_subset_range(ti.begin(), ti.end());

        auto ng = [&](const AMatrix & t) {
            return boost::make_iterator_range(
                boost::make_zip_iterator(boost::make_tuple(
                    subsets.first,
                    boost::make_transform_iterator(m_subs_dists.begin(),
                                                   utils::remove_reference()))),
                boost::make_zip_iterator(boost::make_tuple(
                    subsets.second,
                    boost::make_transform_iterator(
                        m_subs_dists.end(), utils::remove_reference()))));
        };

        auto obj_fun = [&](const AMatrix & m, const Move & t) {
            return this->gain(t);
        };

        auto su = [&](AMatrix & m, const Move & t) {
            this->contract(m, get<0>(t));
            res.push_back(m_nearest_vertex[get<0>(t)]);
            return true;
        };

        auto sc = local_search::make_search_components(ng, obj_fun, su);

        auto lsSolution = data_structures::metric_to_bgl_with_index(
            m_metric, m_voronoi.get_generators(), m_t_idx);

        fill_sub_dists();

        find_save(lsSolution);
        local_search::local_search(lsSolution,
                                   local_search::best_improving_strategy{},
                                   [ = ](AMatrix & a) {
            this->find_save(a);
            return true;
        },
                                   utils::always_false(), sc);

        unique_res(res);
        boost::copy(res, out);
    }

  private:

    // Spanning tree types
    typedef boost::property<boost::edge_index_t, int,
                            boost::property<boost::edge_weight_t, Dist>>
        SpanningTreeEdgeProp;
    typedef boost::subgraph<boost::adjacency_list<
        boost::listS, boost::vecS, boost::undirectedS, boost::no_property,
        SpanningTreeEdgeProp>> SpanningTree;
    typedef boost::graph_traits<SpanningTree> gtraits;
    typedef typename gtraits::edge_descriptor SEdge;

    // Adjacency Matrix types
    typedef typename data_structures::adjacency_matrix<Metric>::type AMatrix;
    typedef boost::graph_traits<AMatrix> mtraits;
    typedef typename mtraits::edge_descriptor MEdge;

    // other types
    typedef std::vector<Dist> ThreeSubsetsDists;
    typedef std::unordered_map<ThreeTuple, VertexType, boost::hash<ThreeTuple>>
        NearstByThreeSubsets;

    template <typename Iter>
    std::pair<data_structures::subsets_iterator<SUSBSET_SIZE, Iter>,
              data_structures::subsets_iterator<SUSBSET_SIZE, Iter>>
    make_three_subset_range(Iter b, Iter e) {
        return data_structures::make_subsets_iterator_range<SUSBSET_SIZE>(b, e);
    }

    void unique_res(ResultSteinerVertices &res) {
        std::sort(res.begin(), res.end());
        auto newEnd = std::unique(res.begin(), res.end());
        res.resize(std::distance(res.begin(), newEnd));
    }

    void contract(AMatrix &am, const ThreeTuple &t) {
        utils::contract(am, get<0>(t), std::get<1>(t));
        utils::contract(am, get<1>(t), std::get<2>(t));
    }

    Dist gain(const Move &t) {
        auto const &m = m_save;
        Idx a, b, c;
        std::tie(a, b, c) = get<0>(t);

        assert(m(a, b) == m(b, c) || m(b, c) == m(c, a) || m(c, a) == m(a, b));
        return this->max3(m(a, b), m(b, c), m(c, a)) +
               this->min3(m(a, b), m(b, c), m(c, a)) - get<1>(t);
    }

    void fill_sub_dists() {
        auto ti = boost::irange<int>(0, N);

        auto subRange = make_three_subset_range(ti.begin(), ti.end());
        m_subs_dists.reserve(std::distance(subRange.first, subRange.second));

        // finding nearest vertex to subset
        for (const ThreeTuple &subset : boost::make_iterator_range(subRange)) {
            // TODO awful coding, need to be changed to loop
            // TODO There is possible problem, one point could belong to two
            // voronoi regions
            // In our implementation the point will be in exactly one region and
            // there
            // it will not be contained in the range
            auto vRange1 = m_voronoi.get_vertices_for_generator(
                m_t_idx.get_val(std::get<0>(subset)));
            auto vRange2 = m_voronoi.get_vertices_for_generator(
                m_t_idx.get_val(std::get<1>(subset)));
            auto vRange3 = m_voronoi.get_vertices_for_generator(
                m_t_idx.get_val(std::get<2>(subset)));
            auto range = boost::join(boost::join(vRange1, vRange2), vRange3);

            if (boost::empty(range)) {
                m_nearest_vertex[subset] = *m_voronoi.get_vertices().begin();
            } else {
                m_nearest_vertex[subset] = *std::min_element(
                    boost::begin(range), boost::end(range),
                    utils::make_functor_to_comparator([&](VertexType v) {
                    return this->dist(v, subset);
                }));
            }
            m_subs_dists.push_back(
                this->dist(m_nearest_vertex[subset], subset));
        }
    }

    Dist max3(Dist a, Dist b, Dist c) { return std::max(std::max(a, b), c); }

    Dist min3(Dist a, Dist b, Dist c) { return std::min(std::min(a, b), c); }

    Dist dist(VertexType steinerPoint, Idx terminalIdx) {
        return m_metric(steinerPoint, m_t_idx.get_val(terminalIdx));
    }

    // minor TODO could by more general somewhere
    Dist dist(VertexType steinerPoint, const ThreeTuple &tup) {
        return dist(steinerPoint, std::get<0>(tup)) +
               dist(steinerPoint, std::get<1>(tup)) +
               dist(steinerPoint, std::get<2>(tup));
    }

    /**
     * @brief Constructs spanning tree from current am
     *
     * @return
     */
    SpanningTree get_spanning_tree(const AMatrix &am) {
        // compute spanning tree and write it to  vector
        std::vector<Idx> pm(N);
        boost::prim_minimum_spanning_tree(am, &pm[0]);

        // transform vector into SpanningTree object
        auto const &weight_map = get(boost::edge_weight, am);
        SpanningTree spanningTree(N);
        for (Idx from = 0; from < N; ++from) {
            if (from != pm[from]) {
                bool succ = add_edge(
                    from, pm[from],
                    SpanningTreeEdgeProp(
                        from, get(weight_map, edge(from, pm[from], am).first)),
                    spanningTree).second;
                assert(succ);
            }
        }
        return spanningTree;
    }

    template <typename WeightMap, typename EdgeRange>
    SEdge max_edge(EdgeRange range, const WeightMap &weight_map) const {
        assert(range.first != range.second);
        return *std::max_element(range.first, range.second,
                                 [&](SEdge e, SEdge f) {
            return get(weight_map, e) < get(weight_map, f);
        });
    }

    void create_subgraphs(SpanningTree &g, SpanningTree &G1, SpanningTree &G2) {
        int n = num_vertices(g);
        std::vector<Idx> comps(n);
        boost::connected_components(g, &comps[0]);
        int c1 = comps[0];
        int c2 = -1;

        for (int i = 0; i < n; ++i) {
            if (comps[i] == c1) {
                add_vertex(g.local_to_global(i), G1);
            } else {
                assert(c2 == -1 || comps[i] == c2);
                c2 = comps[i];
                add_vertex(g.local_to_global(i), G2);
            }
        }
    }

    // setting m_save(v,w) = maxDist, for each v in G1 and w in G2
    void move_save(const SpanningTree &G1, const SpanningTree &G2,
                   Dist maxDist) {
        auto v1 = vertices(G1);
        auto v2 = vertices(G2);
        for (auto v : boost::make_iterator_range(v1)) {
            for (auto w : boost::make_iterator_range(v2)) {
                auto vg = G1.local_to_global(v);
                auto wg = G2.local_to_global(w);
                m_save(vg, wg) = maxDist;
                m_save(wg, vg) = maxDist;
            }
        }
    }

    // finds the longest edge between each pair of vertices
    // in the spanning tree
    // preforms recursive procedure
    void find_save(const AMatrix &am) {
        auto spanningTree = get_spanning_tree(am);

        std::stack<SpanningTree *> s;
        s.push(&spanningTree);

        while (!s.empty()) {
            // TODO delete children at once
            SpanningTree &g = *s.top();
            s.pop();
            int n = num_vertices(g);
            if (n == 1) {
                continue;
            }
            auto eRange = edges(g);
            assert(eRange.first != eRange.second);
            auto const &weight_map = get(boost::edge_weight, g);
            SEdge maxEl = max_edge(eRange, weight_map);
            Dist maxDist = get(weight_map, maxEl);
            remove_edge(maxEl, g);
            SpanningTree &G1 = g.create_subgraph();
            SpanningTree &G2 = g.create_subgraph();
            create_subgraphs(g, G1, G2);

            move_save(G1, G2, maxDist);

            s.push(&G1);
            s.push(&G2);
        }
    }

    const Metric &m_metric;
    const voronoi &m_voronoi;
    ThreeSubsetsDists m_subs_dists;
    NearstByThreeSubsets m_nearest_vertex;
    int N;
    data_structures::array_metric<Dist> m_save;
    data_structures::bimap<VertexType> m_t_idx;
};
} // !detail

/**
 * @brief 11/6 approximation for steiner_tree problem
 *
 * @tparam Metric
 * @tparam voronoi
 * @tparam OutputIterator
 * @param m
 * @param v
 * @param out
 */
template <typename Metric, typename voronoi, typename OutputIterator>
void steiner_tree_zelikovsky11per6approximation(const Metric &m,
                                                const voronoi &v,
                                                OutputIterator out) {
    detail::steiner_tree<Metric, voronoi> st(m, v);
    st.get_result_steiner_vertices(out);
}

} // paal
