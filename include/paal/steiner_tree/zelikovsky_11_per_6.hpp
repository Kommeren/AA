/**
 * @file zelikovsky_11_per_6.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-07-24
 */
#include <unordered_map>
#include <stack>

#define BOOST_RESULT_OF_USE_DECLTYPE

//hack for clang compilation (hash for boost tuple, boost::1_55)
#include <boost/config.hpp>
#undef BOOST_NO_CXX11_HDR_TUPLE

#include <boost/range/join.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/subgraph.hpp>
#include <boost/graph/prim_minimum_spanning_tree.hpp>
#include <boost/range/irange.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/iterator/zip_iterator.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/functional/hash.hpp>

#include "paal/utils/contract_bgl_adjacency_matrix.hpp"
#include "paal/data_structures/subset_iterator.hpp"
#include "paal/data_structures/metric/metric_to_bgl.hpp"
#include "paal/data_structures/metric/metric_traits.hpp"
#include "paal/data_structures/voronoi/voronoi.hpp"
#include "paal/data_structures/metric/graph_metrics.hpp"
#include "paal/local_search/local_search.hpp"
#include "paal/utils/functors.hpp"

namespace paal {
namespace steiner_tree {


namespace detail {
/**
 * class SteinerTree
 * @brief This is Alexander Zelikovsky 11/6 approximation algorithm for steiner tree.
 *
 * Example: <br>
   \snippet zelikovsky_11_per_6_example.cpp Steiner Tree Example
 *
 * complete example is steiner_tree_example.cpp
 *
 * @tparam Metric we only use this metric for distances  (Steiner, Terminal) and (Terminal, Terminal)
 * @tparam Voronoi models WeakVronoi (see \ref voronoi). This is a voronoi division where generators are terminals  of the steiner tree.
 */
template <typename Metric, typename Voronoi>
class SteinerTree {
    typedef int Idx;
public:
    typedef data_structures::MetricTraits<Metric> MT;
    typedef typename MT::DistanceType Dist;
    typedef typename MT::VertexType VertexType;
    static const int SUSBSET_SIZE = 3;

    typedef typename utils::kTuple<Idx, SUSBSET_SIZE>::type ThreeTuple;
    typedef boost::tuple<ThreeTuple, Dist> Move;
    typedef std::vector<VertexType> ResultSteinerVertices;

    /**
     * @brief
     *
     * @param m
     * @param voronoi
     */
    SteinerTree(const Metric & m, const Voronoi & voronoi) :
        m_metric(m), m_voronoi(voronoi), N(std::distance(voronoi.getGenerators().begin(), voronoi.getGenerators().end())),
            m_save(N)  {}


    template <typename OutputIterator>
    void getResultSteinerVertices(OutputIterator out) {
        ResultSteinerVertices res;

        if(m_voronoi.getVertices().empty()) {
            return ;
        }

        auto ti = boost::irange<int>(0, N);
        auto subsets = this->makeThreeSubsetRange(ti.begin(), ti.end());

        auto ng = [&](const AMatrix & t) {
            return std::make_pair(boost::make_zip_iterator(boost::make_tuple(subsets.first,
                                        boost::make_transform_iterator(m_subsDists.begin(), utils::RemoveReference()))),
                                  boost::make_zip_iterator(boost::make_tuple(subsets.second,
                                        boost::make_transform_iterator(m_subsDists.end(), utils::RemoveReference()))));
        };

        auto obj_fun = [&](const AMatrix & m, const Move &t) {return this->gain(t);};

        auto su = [&](AMatrix & m, const Move & t) {
            this->contract(m, get<0>(t));
            res.push_back(m_nearestVertex[get<0>(t)]);
            return true;
        };

        auto sc = local_search::make_SearchComponents(ng, obj_fun, su);

        auto lsSolution = data_structures::metricToBGLWithIndex(
                        m_metric,
                        m_voronoi.getGenerators().begin(),
                        m_voronoi.getGenerators().end(),
                        m_tIdx);

        fillSubDists();

        findSave(lsSolution);
        local_search::local_search<local_search::search_strategies::SteepestSlope>(
                lsSolution,
                [&](AMatrix & a){this->findSave(a);},
                utils::ReturnFalseFunctor(),
                sc);

        uniqueRes(res);
        boost::copy(res, out);
    }

private:

    //Spanning tree types
    typedef boost::property<boost::edge_index_t, int, boost::property<boost::edge_weight_t, Dist>>  SpanningTreeEdgeProp;
    typedef boost::subgraph<boost::adjacency_list<boost::listS, boost::vecS,
                boost::undirectedS, boost::no_property, SpanningTreeEdgeProp>> SpanningTree;
    typedef boost::graph_traits<SpanningTree> gtraits;
    typedef typename gtraits::edge_descriptor SEdge;

    //Adjacency Matrix types
    typedef typename data_structures::AdjacencyMatrix<Metric>::type AMatrix;
    typedef boost::graph_traits<AMatrix> mtraits;
    typedef typename mtraits::edge_descriptor MEdge;

    //other types
    typedef std::vector<Dist> ThreeSubsetsDists;
    typedef std::unordered_map<ThreeTuple, VertexType, boost::hash<ThreeTuple>> NearstByThreeSubsets;

        template <typename Iter> std::pair<data_structures::SubsetsIterator<SUSBSET_SIZE, Iter>,
                                           data_structures::SubsetsIterator<SUSBSET_SIZE, Iter>>
    makeThreeSubsetRange(Iter b, Iter e) {
        return data_structures::make_SubsetsIteratorRange<SUSBSET_SIZE>(b,e);
    }

    void uniqueRes(ResultSteinerVertices & res) {
        std::sort(res.begin(), res.end());
        auto newEnd = std::unique(res.begin(), res.end());
        res.resize(std::distance(res.begin(), newEnd));
    }

    void contract(AMatrix & am ,const ThreeTuple & t) {
        utils::contract(am, get<0>(t), std::get<1>(t));
        utils::contract(am, get<1>(t), std::get<2>(t));
    }

    Dist gain(const Move & t){
        auto const & m = m_save;
        Idx a,b,c;
        std::tie(a, b, c) = get<0>(t);

        assert(m(a,b) == m(b,c) || m(b,c) == m(c, a) || m(c,a) == m(a,b));
        return this->max3(m(a, b), m(b, c), m(c,a)) + this->min3(m(a, b), m(b, c), m(c,a)) - get<1>(t);
    }

    void fillSubDists() {
        auto ti = boost::irange<int>(0, N);

        auto subRange = makeThreeSubsetRange(ti.begin(), ti.end());
        m_subsDists.reserve(std::distance(subRange.first, subRange.second));

        //finding nearest vertex to subset
        for(const ThreeTuple & subset : boost::make_iterator_range(subRange)) {
            //TODO awful coding, need to be changed to loop
            //TODO There is possible problem, one point could belong to two Voronoi regions
            //In our implementation the point will be in exactly one region and there
            //it will not be contained in the range
            auto vRange1 =  m_voronoi.getVerticesForGenerator(m_tIdx.getVal(std::get<0>(subset)));
            auto vRange2 =  m_voronoi.getVerticesForGenerator(m_tIdx.getVal(std::get<1>(subset)));
            auto vRange3 =  m_voronoi.getVerticesForGenerator(m_tIdx.getVal(std::get<2>(subset)));
            auto range = boost::join(boost::join(vRange1, vRange2), vRange3);

            if(boost::empty(range)) {
                m_nearestVertex[subset] = *m_voronoi.getVertices().begin();
            } else {
                m_nearestVertex[subset] =  *std::min_element(boost::begin(range), boost::end(range),
                        utils::make_FunctorToComparator([&](VertexType v){return this->dist(v, subset);}));
            }
            m_subsDists.push_back(this->dist(m_nearestVertex[subset], subset));
        }
    }

    Dist max3(Dist a, Dist b, Dist c) {
        return std::max(std::max(a,b),c);
    }

    Dist min3(Dist a, Dist b, Dist c) {
        return std::min(std::min(a,b),c);
    }

    Dist dist(VertexType steinerPoint, Idx terminalIdx) {
        return   m_metric(steinerPoint, m_tIdx.getVal(terminalIdx));
    }

    //minor TODO could by more general somewhere
    Dist dist(VertexType steinerPoint, const ThreeTuple & tup) {
        return   dist(steinerPoint, std::get<0>(tup))
               + dist(steinerPoint, std::get<1>(tup))
               + dist(steinerPoint, std::get<2>(tup));
    }

    /**
     * @brief Constructs spanning tree from current am
     *
     * @return
     */
    SpanningTree getSpanningTree(const AMatrix & am) {
        //compute spanning tree and write it to  vector
        std::vector<Idx> pm(N);
        boost::prim_minimum_spanning_tree(am, &pm[0]);

        //transform vector into SpanningTree object
        auto const  & weight_map = get(boost::edge_weight, am);
        SpanningTree spanningTree(N);
        for(Idx from = 0; from < N; ++from){
            if(from != pm[from]) {
                bool succ =add_edge(from, pm[from],
                    SpanningTreeEdgeProp(from, get(weight_map, edge(from, pm[from], am).first)), spanningTree).second;
                assert(succ);
            }
        }
        return spanningTree;
    }

        template <typename WeightMap, typename EdgeRange>
    SEdge maxEdge(EdgeRange range, const WeightMap & weight_map) const {
        assert(range.first != range.second);
        return *std::max_element(range.first, range.second, [&](SEdge e, SEdge f){
            return get(weight_map, e) <
                   get(weight_map, f);
        });
    }

    void createSubgraphs(SpanningTree & g, SpanningTree & G1, SpanningTree & G2) {
        int n = num_vertices(g);
        std::vector<Idx> comps(n);
        boost::connected_components(g, &comps[0]);
        int c1 = comps[0];
        int c2 = -1;

        for(int i = 0; i < n; ++i) {
            if(comps[i] == c1) {
                add_vertex(g.local_to_global(i), G1);
            } else {
                assert(c2 == -1 || comps[i] == c2);
                c2 = comps[i];
                add_vertex(g.local_to_global(i), G2);
            }
        }
    }

    //setting m_save(v,w) = maxDist, for each v in G1 and w in G2
    void moveSave(const SpanningTree & G1, const SpanningTree & G2, Dist maxDist) {
        auto v1 = vertices(G1);
        auto v2 = vertices(G2);
        for(auto v : boost::make_iterator_range(v1)) {
            for(auto w : boost::make_iterator_range(v2)) {
                auto vg = G1.local_to_global(v);
                auto wg = G2.local_to_global(w);
                m_save(vg, wg) =  maxDist;
                m_save(wg, vg) =  maxDist;
            }
        }
    }

    //finds the longest edge between each pair of vertices
    //in the spanning tree
    //preforms recursive procedure
    void findSave(const AMatrix & am) {
        auto spanningTree = getSpanningTree(am);

        std::stack<SpanningTree *> s;
        s.push(&spanningTree);

        while(!s.empty()) {
            //TODO delete children at once
            SpanningTree & g = *s.top();
            s.pop();
            int n = num_vertices(g);
            if(n == 1) {
                continue;
            }
            auto eRange = edges(g);
            assert(eRange.first != eRange.second);
            auto const  & weight_map = get(boost::edge_weight, g);
            SEdge maxEl = maxEdge(eRange, weight_map);
            Dist maxDist = get(weight_map, maxEl);
            remove_edge(maxEl, g);
            SpanningTree & G1 = g.create_subgraph();
            SpanningTree & G2 = g.create_subgraph();
            createSubgraphs(g, G1, G2);

            moveSave(G1, G2, maxDist);

            s.push(&G1);
            s.push(&G2);
        }
    }

    const Metric & m_metric;
    const Voronoi & m_voronoi;
    ThreeSubsetsDists m_subsDists;
    NearstByThreeSubsets m_nearestVertex;
    int N;
    data_structures::ArrayMetric<Dist> m_save;
    data_structures::BiMap<VertexType> m_tIdx;
};
} // !detail

template <typename Metric, typename Voronoi, typename OutputIterator>
void steinerTreeZelikovsky11per6approximation(const Metric & m, const Voronoi & v, OutputIterator out) {
    detail::SteinerTree<Metric, Voronoi> st(m, v);
    st.getResultSteinerVertices(out);
}


} // steiner_tree
} //paal
