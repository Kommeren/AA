#include <map>
#include <stack>

#include <boost/range/join.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/subgraph.hpp>
#include <boost/graph/prim_minimum_spanning_tree.hpp>
#include <boost/range/irange.hpp>

#include "paal/helpers/iterator_helpers.hpp"
#include "paal/helpers/subset_iterator.hpp"
#include "paal/helpers/metric_to_bgl.hpp"
#include "paal/helpers/functors_to_paal_functors.hpp"
#include "paal/helpers/bimap.hpp"
#include "paal/helpers/metric_on_idx.hpp"

#include "paal/data_structures/voronoi.hpp"
#include "paal/data_structures/graph_metrics.hpp"
#include "paal/local_search/local_search.hpp"

namespace paal {
namespace local_search {
namespace steiner_tree {

template <typename Metric, typename Voronoi> 
class SteinerTree {
public:
    typedef typename Metric::DistanceType Dist;
    typedef typename Metric::VertexType VertexType;
    static const int SUSBSET_SIZE = 3;

    typedef typename helpers::kTuple<VertexType, SUSBSET_SIZE>::type ThreeTuple;
    typedef std::vector<VertexType> ResultSteinerVertices;
       
    SteinerTree(const Metric & m, const Voronoi & voronoi) : 
        m_metric(m), m_voronoi(voronoi), N(std::distance(voronoi.getGenerators().begin(), voronoi.getGenerators().end())), 
            m_save(N), m_tIdx(voronoi.getGenerators().begin(), voronoi.getGenerators().end()), m_idxMetric(m_metric, m_tIdx) {}


    ResultSteinerVertices getSteinerTree() {
        ResultSteinerVertices res;
        
        if(m_voronoi.getVertices().empty()) {
            return res;
        }

        fillSubDists();


        auto ti = boost::irange<int>(0, N);
        AMatrix g = metricToBGL(m_idxMetric, ti.begin(), ti.end());
        
        auto obj_fun = std::bind(std::mem_fun(&SteinerTree::gain), this, std::placeholders::_1);

        auto ng = [&](const ThreeTuple & t){
            return this->makeThreeSubsetRange(ti.begin(), ti.end());
        };

        typedef FunctToNeigh<decltype(ng)> NG; 
        
        typedef local_search::LocalSearchFunctionStep<ThreeTuple, NG, decltype(obj_fun),
              TrivialSolutionUpdater, TrivialStopCondition, search_strategies::SteepestSlope>  LS;

        LS ls(ThreeTuple(), NG(ng), obj_fun);

        while(true) {
        
            findSave(g);

            //TODO not optimal because of logharitmic gain
            if(!ls.search())
                break;

            const ThreeTuple & best = ls.getSolution();
            contract(best, g);   
            res.push_back(m_nearestVertex[best]);
        }

        return res; 
    }
private:
    //Spanning tree types
    typedef boost::property<boost::edge_index_t, int, boost::property<boost::edge_weight_t, Dist>>  SpanningTreeEdgeProp;
    typedef boost::subgraph<boost::adjacency_list<boost::listS, boost::vecS, 
                boost::undirectedS, boost::no_property, SpanningTreeEdgeProp>> SpanningTree;
    typedef boost::graph_traits<SpanningTree> gtraits;
    typedef typename gtraits::edge_descriptor SEdge;
    
    //Adjeny Matrix types
    typedef typename AdjacencyMatrix<Metric>::type AMatrix;
    typedef boost::graph_traits<AMatrix> mtraits;
    typedef typename mtraits::edge_descriptor MEdge;
   
    //other types
    typedef std::map<ThreeTuple, Dist> ThreeSubsetsDists;
    typedef std::map<ThreeTuple, VertexType> NearstByThreeSubsets;

        template <typename Iter> std::pair<helpers::SubsetsIterator<Iter,SUSBSET_SIZE>, 
                                           helpers::SubsetsIterator<Iter,SUSBSET_SIZE>>        
    makeThreeSubsetRange(Iter b, Iter e) {
        return helpers::make_SubsetsIteratorrange<Iter, SUSBSET_SIZE>(b,e);
    }

    void contract(const ThreeTuple & t, AMatrix & m) {
        contract(std::get<0>(t), std::get<1>(t), m);
        contract(std::get<1>(t), std::get<2>(t), m);
    }
   

    //TODO put outside
    void contract(VertexType v, VertexType w, AMatrix & m) {
        auto const & weight_map = boost::get(boost::edge_weight, m);
        weight_map[boost::edge(v, w, m).first] = 0;
        for(const MEdge & e : helpers::make_range(boost::out_edges(v, m))) {
            MEdge  f = boost::edge(w, boost::target(e, m), m).first;
            auto & we = weight_map[e];
            auto & wf = weight_map[f];
            we = std::min(we, wf);
            wf = we;

            // TODO hide  checkking
            auto  teste = boost::edge(boost::target(e, m), w, m).first;
            auto  testf = boost::edge(boost::target(e, m), v, m).first;
            auto wte = weight_map[teste];
            auto wtf = weight_map[testf];
            assert(wte == wtf && wte == we);

        }
    }
        
    Dist gain(const ThreeTuple & t){
        auto const  & m = m_save;
        VertexType a,b,c;
        std::tie(a, b, c) = t;
        
        assert(m(a,b) == m(b,c) || m(b,c) == m(c, a) || m(c,a) == m(a,b));
        return this->max3(m(a, b), m(b, c), m(c,a)) + this->min3(m(a, b), m(b, c), m(c,a)) - m_subsDists[t];
    }

    void fillSubDists() {
        auto ti = boost::irange<int>(0, N);

        auto subRange = makeThreeSubsetRange(ti.begin(), ti.end());
        
        //finding nearest vertex to subset
        for(const ThreeTuple & subset : helpers::make_range(subRange)) {
            //TODO awfull coding, need to be changed to loop
            auto vRange1 =  m_voronoi.getVerticesForGenerator(std::get<0>(subset));
            auto vRange2 =  m_voronoi.getVerticesForGenerator(std::get<1>(subset));
            auto vRange3 =  m_voronoi.getVerticesForGenerator(std::get<2>(subset));
            auto range = boost::join(boost::join(vRange1, vRange2), vRange3);
            
            if(boost::empty(range)) {
                m_nearestVertex[subset] = *m_voronoi.getVertices().begin();
            } else {
                m_nearestVertex[subset] =  *std::min_element(boost::begin(range), boost::end(range), [&](VertexType v1, VertexType v2) {
                    return this->dist(v1, subset) < this->dist(v2, subset);
                });
            }
            m_subsDists[subset] = this->dist(m_nearestVertex[subset], subset);
        }
    }

    VertexType max3(VertexType a, VertexType b, VertexType c) {
        return std::max(std::max(a,b),c);
    }
    
    VertexType min3(VertexType a, VertexType b, VertexType c) {
        return std::min(std::min(a,b),c);
    }

    Dist dist(VertexType steinerPoint, VertexType terminalIdx) {
        return   m_metric(steinerPoint, m_tIdx.getVal(terminalIdx));
    }

    //minor TODO could by more general somewhere
    Dist dist(VertexType steinerPoint, const ThreeTuple & tup) {
        return   dist(steinerPoint, std::get<0>(tup))
               + dist(steinerPoint, std::get<1>(tup))
               + dist(steinerPoint, std::get<2>(tup));
    }

    SpanningTree getSpanningTree(const AMatrix & g) {
        //spanning tree to vector
        std::vector<VertexType> pm(N);
        boost::prim_minimum_spanning_tree(g, &pm[0]);
        
        //vector to SpanningTree
        SpanningTree spanningTree(N);
        for(VertexType from = 0; from < N; ++from){
            if(from != pm[from]) {
                bool succ =boost::add_edge(from, pm[from], 
                    SpanningTreeEdgeProp(from, m_idxMetric(from,pm[from])), spanningTree).second;
                assert(succ);
            }
        }
        return spanningTree;
    }
    
        template <typename WeightMap, typename EdgeRange> 
    SEdge maxEdge(EdgeRange range, const WeightMap & weight_map) const {
        assert(range.first != range.second);
        return *std::max_element(range.first, range.second, [&](SEdge e, SEdge f){
            return boost::get(weight_map, e) < 
                   boost::get(weight_map, f);
        });
    }

    void createSubgraphs(SpanningTree & g, SpanningTree & G1, SpanningTree & G2) {
        int n = boost::num_vertices(g);
        std::vector<VertexType> comps(n);
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

    void updateSave(const SpanningTree & G1, const SpanningTree & G2, Dist maxDist) {
        auto v1 = vertices(G1);
        auto v2 = vertices(G2);
        for(VertexType v : helpers::make_range(v1)) {
            for(VertexType w : helpers::make_range(v2)) {
                auto vg = G1.local_to_global(v);
                auto wg = G2.local_to_global(w);
                m_save.set(vg, wg, maxDist);
                m_save.set(wg, vg, maxDist);
            }
        }
    }

    void findSave(const AMatrix & am) {
        auto spanningTree = getSpanningTree(am);
        
        std::stack<SpanningTree *> s;
        s.push(&spanningTree);

        while(!s.empty()) {
            //TODO delete children at once
            SpanningTree & g = *s.top();
            s.pop();
            int n = boost::num_vertices(g);
            if(n == 1) {   
                continue;
            }
            auto eRange = boost::edges(g);
            assert(eRange.first != eRange.second);
            auto const  & weight_map = boost::get(boost::edge_weight, g);
            SEdge maxEl = maxEdge(eRange, weight_map);
            Dist maxDist = boost::get(weight_map, maxEl);
            boost::remove_edge(maxEl, g);
            SpanningTree & G1 = g.create_subgraph();
            SpanningTree & G2 = g.create_subgraph();
            createSubgraphs(g, G1, G2);
    
            updateSave(G1, G2, maxDist);

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
    helpers::BiMap<VertexType> m_tIdx;
    helpers::MetricOnIdx<Metric> m_idxMetric;
};

} // steiner_tree
} //local_search
} //paal
