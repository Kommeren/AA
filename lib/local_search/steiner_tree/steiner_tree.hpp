#include <map>
#include <stack>

#include <boost/range/join.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/subgraph.hpp>
#include <boost/graph/prim_minimum_spanning_tree.hpp>

#include "helpers/iterator_helpers.hpp"
#include "helpers/subset_iterator.hpp"
#include "helpers/metric_to_bgl.hpp"
#include "helpers/functors_to_paal_functors.hpp"
#include "data_structures/voronoi.hpp"
#include "data_structures/graph_metrics.hpp"
#include "local_search/local_search.hpp"

namespace paal {
namespace local_search {
namespace steiner_tree {

template <typename Metric, typename Voronoi> 
class SteinerTree {
public:
    typedef typename Metric::DistanceType Dist;
    typedef typename Metric::VertexType VertexType;
    typedef std::vector<VertexType> Dists;
    static const int SUSBSET_SIZE = 3;
    typedef typename kTuple<VertexType, SUSBSET_SIZE>::type ThreeTuple;
//    typedef std::vector<ThreeTuple> ThreeSubsetsToIndex;
//    typedef std::map<ThreeTuple, int> IndexToThreeSubsets;
    typedef std::map<ThreeTuple, Dist> ThreeSubsetsDists;
    typedef std::map<ThreeTuple, VertexType> NearstByThreeSubsets;
    typedef boost::property<boost::edge_index_t, int, boost::property<boost::edge_weight_t, Dist>>  EdgeProp;
    typedef boost::subgraph<boost::adjacency_list<boost::listS, boost::vecS, boost::undirectedS, boost::no_property, EdgeProp>> GraphType;
    typedef std::vector<VertexType> ResultSteinerVertices;
    typedef decltype(std::declval<Voronoi>().getGenerators().begin()) TerminalIterator;

//    typedef GraphType GraphType;
    
    SteinerTree(const Metric & m, const Voronoi & voronoi) : 
        m_metric(m), m_voronoi(voronoi), N(std::distance(voronoi.getGenerators().begin(), voronoi.getGenerators().end())), 
            m_save(N) {}


    ResultSteinerVertices getSteinerTree() {
        auto const & terminals = m_voronoi.getGenerators();
        auto const & vertices = m_voronoi.getVertices();
        typedef decltype(vertices.begin()) SteinerIterator;
        typedef helpers::SubsetsIterator<TerminalIterator, SUSBSET_SIZE> ThreeSubsetsIter;
        //ThreeSubsetsToIndex subToIndex;
        ThreeSubsetsDists subsDists;
        NearstByThreeSubsets nearestVertex;
        ResultSteinerVertices res;

        fillSubDists(subsDists, nearestVertex);

        auto g = metricToBGL(m_metric, terminals.begin(), terminals.end());
        
        findSave(g);
        
        auto obj_fun = [&](const ThreeTuple & t){
            auto const  & m = m_save;
            VertexType a,b,c;
            std::tie(a, b, c) = t;
            assert(a == b || b == c || c == a);
            return this->max3(m(a, b), m(b, c), m(c,a)) + this->min3(m(a, b), m(b, c), m(c,a)) - subsDists[t];
        };

        auto ng = [&](const ThreeTuple & t){
            return helpers::make_SubsetsIteratorrange<TerminalIterator, SUSBSET_SIZE>(terminals.begin(), terminals.end());
        };

        typedef FunctToNeigh<decltype(ng)> NG; 
        
        local_search::LocalSearchFunctionStep<ThreeTuple, NG, decltype(obj_fun),
            TrivialSolutionUpdater, TrivialStopCondition, search_strategies::SteepestSlope> 
                ls(*ThreeSubsetsIter(terminals.begin(), terminals.end()), NG(ng), obj_fun);

        ls.search();

        return res; 
    }
private:
    typedef typename AdjacencyMatrix<Metric>::type AMatrix;
    typedef boost::graph_traits<GraphType> gtraits;
    typedef typename gtraits::edge_iterator geIter;
    typedef typename gtraits::edge_descriptor Edge;

    void fillSubDists(ThreeSubsetsDists & subsDists,
                      NearstByThreeSubsets & nearestVertex) {
        auto const & terminals = m_voronoi.getGenerators();

        auto subRange = helpers::make_SubsetsIteratorrange<TerminalIterator, SUSBSET_SIZE>(terminals.begin(), terminals.end());
        
        //finding nearest vertex to subset
        for(const ThreeTuple & subset : helpers::make_range(subRange)) {
            std::cout << std::get<0>(subset) << std::endl;
            //TODO awfull coding, need to be changed to loop
            auto vRange1 =  m_voronoi.getVerticesForGenerator(std::get<0>(subset));
            auto vRange2 =  m_voronoi.getVerticesForGenerator(std::get<1>(subset));
            auto vRange3 =  m_voronoi.getVerticesForGenerator(std::get<2>(subset));
            auto range = boost::join(boost::join(vRange1, vRange2), vRange3);
            //int k = subToIndex.push_back(subsIndex);
            //indexToSub.insert(std::make_pair(subset, k));
            nearestVertex[subset] =  *std::min_element(boost::begin(range), boost::end(range), [&](VertexType v1, VertexType v2) {
                return this->dist(v1, subset) < this->dist(v2, subset);
            });
            subsDists[subset] = this->dist(nearestVertex[subset], subset);
        }
    }

    VertexType max3(VertexType a, VertexType b, VertexType c) {
        return std::max(std::max(a,b),c);
    }
    
    VertexType min3(VertexType a, VertexType b, VertexType c) {
        return std::min(std::min(a,b),c);
    }

    //minor TODO could by more general somewhere
    Dist dist(VertexType v, const ThreeTuple & tup) {
        return   m_metric(v, std::get<0>(tup))
               + m_metric(v, std::get<1>(tup))
               + m_metric(v, std::get<2>(tup));
    }

    GraphType getSpanningTree(const AMatrix & g) {
        //spanning tree to vector
        std::vector<VertexType> pm(N, -7);
        boost::prim_minimum_spanning_tree(g, &pm[0]);
        
        //vector to GraphType
        GraphType spanningTree(N);
        for(VertexType from = 0; from < N; ++from){
            if(from != pm[from]) {
                bool succ =boost::add_edge(from, pm[from], 
                    EdgeProp(from, m_metric(from,pm[from])), spanningTree).second;
                assert(succ);
            }
        }
        return spanningTree;
    }
    
        template <typename WeightMap, typename EdgeRange> 
    Edge maxEdge(EdgeRange range, const WeightMap & weight_map) const {
        assert(range.first != range.second);
        return *std::max_element(range.first, range.second, [&](Edge e, Edge f){
            return boost::get(weight_map, e) < 
                   boost::get(weight_map, f);
        });
    }

    void createSubgraphs(GraphType & g, GraphType & G1, GraphType & G2) {
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

    void updateSave(const GraphType & G1, const GraphType & G2, Dist maxDist) {
        auto v1 = vertices(G1);
        auto v2 = vertices(G2);
        std::for_each(v1.first, v1.second, [&](VertexType v){
            std::for_each(v2.first, v2.second, [&](VertexType w){
                m_save.set(G1.local_to_global(v), G2.local_to_global(w), maxDist);
            });
        });
    }

    void findSave(const AMatrix & am) {
        auto spanningTree = getSpanningTree(am);
        
        std::stack<GraphType *> s;
        s.push(&spanningTree);

        while(!s.empty()) {
            //TODO delete children at once
            GraphType & g = *s.top();
            s.pop();
            int n = boost::num_vertices(g);
       //     std::cout << n << std::endl;
            if(n == 1) {   
                continue;
            }
            auto eRange = boost::edges(g);
            assert(eRange.first != eRange.second);
            auto const  & weight_map = boost::get(boost::edge_weight, g);
            Edge maxEl = maxEdge(eRange, weight_map);
            Dist maxDist = boost::get(weight_map, maxEl);
            boost::remove_edge(maxEl, g);
            GraphType & G1 = g.create_subgraph();
            GraphType & G2 = g.create_subgraph();
            createSubgraphs(g, G1, G2);
    
            updateSave(G1, G2, maxDist);

            s.push(&G1);
            s.push(&G2);
        }
    }

    const Metric & m_metric;
    const Voronoi & m_voronoi;
    int N;
    data_structures::MetricBase<Dist> m_save;
};

} // steiner_tree
} //local_search
} //paal
