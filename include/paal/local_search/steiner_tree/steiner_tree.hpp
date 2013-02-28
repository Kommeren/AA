#include <map>
#include <stack>

#include <boost/range/join.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/subgraph.hpp>
#include <boost/graph/prim_minimum_spanning_tree.hpp>
#include <boost/range/irange.hpp>
#include <boost/iterator/zip_iterator.hpp>
#include <boost/range/any_range.hpp>

#include "paal/utils/iterator_utils.hpp"
#include "paal/utils/contract_bgl_adjaceny_matrix.hpp"

#include "paal/data_structures/subset_iterator.hpp"
#include "paal/data_structures/metric/metric_to_bgl.hpp"
#include "paal/data_structures/voronoi.hpp"
#include "paal/data_structures/metric/graph_metrics.hpp"
#include "paal/local_search/local_search.hpp"

namespace paal {
namespace local_search {
namespace steiner_tree {

template <typename Metric, typename Voronoi> 
class SteinerTree {
    typedef int Idx;
public:
    typedef typename Metric::DistanceType Dist;
    typedef typename Metric::VertexType VertexType;
    static const int SUSBSET_SIZE = 3;

    typedef typename utils::kTuple<Idx, SUSBSET_SIZE>::type ThreeTuple;
    typedef boost::tuple<const ThreeTuple &, Dist> Update;
    typedef std::vector<VertexType> ResultSteinerVertices;
       
    /**
     * @brief 
     *
     * @param m we only use this metric for distances  (Steiner, Terminal) and (Terminal, Terminal)
     * @param voronoi
     */
    SteinerTree(const Metric & m, const Voronoi & voronoi) : 
        m_metric(m), m_voronoi(voronoi), N(std::distance(voronoi.getGenerators().begin(), voronoi.getGenerators().end())), 
            m_save(N)  {}


    ResultSteinerVertices getResultSteinerVertices() {
        ResultSteinerVertices res;

        if(m_voronoi.getVertices().empty()) {
            return res;
        }

        auto ti = boost::irange<int>(0, N);

        auto ng = [&](const AMatrix & t){
            auto subsets = this->makeThreeSubsetRange(ti.begin(), ti.end());
            return std::make_pair(boost::make_zip_iterator(boost::make_tuple(subsets.first, m_subsDists.begin())), 
                                  boost::make_zip_iterator(boost::make_tuple(subsets.second, m_subsDists.end())));
        };
        
        auto obj_fun = [&](const AMatrix & m, const Update &t) {return this->gain(t);};

        auto su = [&](AMatrix & m, const Update & t) {
            this->contract(m, boost::get<0>(t));
            res.push_back(m_nearestVertex[boost::get<0>(t)]);
        };

        auto sc = make_SearchComponents(ng, obj_fun, su);

        typedef local_search::LocalSearchStep<AMatrix, 
                    decltype(sc), search_strategies::SteepestSlope>  LS;
        LS ls(utils::metricToBGLWithIndex(
                        m_metric, 
                        m_voronoi.getGenerators().begin(), 
                        m_voronoi.getGenerators().end(), 
                        m_tIdx) , sc);
        
        fillSubDists();

        while(true) {
            findSave(ls.getSolution());

            if(!ls.search()) {
                break;
            }
        }

        uniqueRes(res);
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
    typedef typename utils::AdjacencyMatrix<Metric>::type AMatrix;
    typedef boost::graph_traits<AMatrix> mtraits;
    typedef typename mtraits::edge_descriptor MEdge;
   
    //other types
    typedef std::vector<Dist> ThreeSubsetsDists;
    typedef std::map<ThreeTuple, VertexType> NearstByThreeSubsets;

        template <typename Iter> std::pair<data_structures::SubsetsIterator<Iter,SUSBSET_SIZE>, 
                                           data_structures::SubsetsIterator<Iter,SUSBSET_SIZE>>        
    makeThreeSubsetRange(Iter b, Iter e) {
        return data_structures::make_SubsetsIteratorrange<Iter, SUSBSET_SIZE>(b,e);
    }

    void uniqueRes(ResultSteinerVertices & res) {
        std::sort(res.begin(), res.end());
        auto newEnd = std::unique(res.begin(), res.end());
        res.resize(std::distance(res.begin(), newEnd));
    }

    void contract(AMatrix & am ,const ThreeTuple & t) {
        utils::contract(am, std::get<0>(t), std::get<1>(t));
        utils::contract(am, std::get<1>(t), std::get<2>(t));
    }
        
    Dist gain(const Update & t){
        auto const  & m = m_save;
        VertexType a,b,c;
        std::tie(a, b, c) = boost::get<0>(t);
        
        assert(m(a,b) == m(b,c) || m(b,c) == m(c, a) || m(c,a) == m(a,b));
        return this->max3(m(a, b), m(b, c), m(c,a)) + this->min3(m(a, b), m(b, c), m(c,a)) - boost::get<1>(t);
    }

    void fillSubDists() {
        auto ti = boost::irange<int>(0, N);

        auto subRange = makeThreeSubsetRange(ti.begin(), ti.end());
        m_subsDists.reserve(std::distance(subRange.first, subRange.second));
        
        //finding nearest vertex to subset
        for(const ThreeTuple & subset : utils::make_range(subRange)) {
            //TODO awfull coding, need to be changed to loop
            //There is possible problem, one point could be in two Voronoi regions
            //In our implementation the poin will be in exactly o0ne region and there 
            //it will not be contained in the range
            auto vRange1 =  m_voronoi.getVerticesForGenerator(m_tIdx.getVal(std::get<0>(subset)));
            auto vRange2 =  m_voronoi.getVerticesForGenerator(m_tIdx.getVal(std::get<1>(subset)));
            auto vRange3 =  m_voronoi.getVerticesForGenerator(m_tIdx.getVal(std::get<2>(subset)));
            auto range = boost::join(boost::join(vRange1, vRange2), vRange3);
            
            if(boost::empty(range)) {
                m_nearestVertex[subset] = *m_voronoi.getVertices().begin();
            } else {
                m_nearestVertex[subset] =  *std::min_element(boost::begin(range), boost::end(range), [&](VertexType v1, VertexType v2) {
                    return this->dist(v1, subset) < this->dist(v2, subset);
                });
            }
            m_subsDists.push_back(this->dist(m_nearestVertex[subset], subset));
        }
    }

    VertexType max3(VertexType a, VertexType b, VertexType c) {
        return std::max(std::max(a,b),c);
    }
    
    VertexType min3(VertexType a, VertexType b, VertexType c) {
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
     * @brief Costructs spanning tree from curent am
     *
     * @return 
     */
    SpanningTree getSpanningTree(const AMatrix & am) {
        //compute spanning tree and write it to  vector
        std::vector<VertexType> pm(N);
        boost::prim_minimum_spanning_tree(am, &pm[0]);
        
        //transform vector intto SpanningTree object
        auto const  & weight_map = boost::get(boost::edge_weight, am);
        SpanningTree spanningTree(N);
//        std::cout << "Drzewo : " << std::endl;
        for(VertexType from = 0; from < N; ++from){
  //          std::cout << from << " "<< pm[from] << std::endl;
            if(from != pm[from]) {
                bool succ =boost::add_edge(from, pm[from], 
                    SpanningTreeEdgeProp(from, boost::get(weight_map, boost::edge(from, pm[from], am).first)), spanningTree).second;
                assert(succ);
            }
        }
//        std::cout << "End " << std::endl;
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
        for(VertexType v : utils::make_range(v1)) {
            for(VertexType w : utils::make_range(v2)) {
                auto vg = G1.local_to_global(v);
                auto wg = G2.local_to_global(w);
                m_save(vg, wg) =  maxDist;
                m_save(wg, vg) =  maxDist;
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
    data_structures::BiMap<VertexType> m_tIdx;
};

} // steiner_tree
} //local_search
} //paal
