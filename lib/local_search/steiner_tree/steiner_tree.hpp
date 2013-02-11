#include <map>

#include <boost/range/join.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/subgraph.hpp>

#include "helpers/iterator_helpers.hpp"
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
    typedef boost::property<boost::edge_index_t, int,boost::property<boost::edge_weight_t, Dist>>  EdgeProp;
    typedef boost::subgraph<boost::adjacency_list<boost::listS, boost::vecS, boost::undirectedS, boost::no_property, EdgeProp>> GraphType;
    typedef std::vector<VertexType> ResultSteinerVertices;

//    typedef GraphType GraphType;
    
    SteinerTree(const Metric & m, const Voronoi & voronoi) : 
        m_metric(m), m_voronoi(voronoi), N(std::distance(voronoi.getGenerators().begin(), voronoi.getGenerators().end())), 
            m_findSaveMatrtix(N) {}


    ResultSteinerVertices getSteinerTree() {
        auto const & terminals = m_voronoi.getGenerators();
        auto const & vertices = m_voronoi.getVertices();
        typedef decltype(terminals.begin()) TerminalIterator;
        typedef decltype(vertices.begin()) SteinerIterator;
        typedef helpers::SubsetsIterator<TerminalIterator, SUSBSET_SIZE> ThreeSubsetsIter;
        //ThreeSubsetsToIndex subToIndex;
        ThreeSubsetsDists subsDists;
        NearstByThreeSubsets nearestVertex;
        ResultSteinerVertices res;

        auto terminalsBegin = terminals.begin();
        auto terminalsEnd   = terminals.end();
        auto steinerBegin   = vertices.begin();
        auto steinerEnd     = vertices.end();

        ThreeSubsetsIter subBegin(terminalsBegin, terminalsEnd);
        ThreeSubsetsIter subEnd(terminalsEnd, terminalsEnd);
        
        //finding nearest vertex to subset
        std::for_each(subBegin, subEnd, [&](const ThreeTuple & subset) {
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
        });

        auto g = metricToBGL(m_metric, terminalsBegin, terminalsEnd);
        
        findSave(g);
        //INCOMPLETE!!!
        auto obj_fun = [&](const ThreeTuple & t){return subsDists[t];};
        auto ng = [&](const ThreeTuple & t){
            return helpers::make_SubsetsIteratorrange<TerminalIterator, SUSBSET_SIZE>(terminalsBegin, terminalsEnd);
        };
        typedef FunctToNeigh<decltype(ng)> NG; 
        
        local_search::LocalSearchFunctionStep<ThreeTuple, NG, decltype(obj_fun),
            TrivialSolutionUpdater, TrivialStopCondition, search_strategies::SteepestSlope> 
                ls(*ThreeSubsetsIter(terminalsBegin, terminalsEnd), NG(ng), obj_fun);

        ls.search();
        return res; 
    }
private:
    typedef typename AdjacencyMatrix<Metric>::type AMatrix;

    //minor TODO could by more general somewhere
    Dist dist(VertexType v, const ThreeTuple & tup) {
        return   m_metric(v, std::get<0>(tup))
               + m_metric(v, std::get<1>(tup))
               + m_metric(v, std::get<2>(tup));
    }

    GraphType getSpanningTree(const AMatrix & g) {
        //spanning tree to vector
      //  std::cout << N <<std::endl;
        std::vector<VertexType> pm(N, -7);
        boost::prim_minimum_spanning_tree(g, &pm[0]);
        
        //vector to GraphType
        GraphType spanningTree(N);
        for(VertexType from = 0; from < N; ++from){
    //        std::cout << from << " ," << pm[from] << std::endl;
            if(from != pm[from]) {
                boost::add_edge(from, pm[from], 
                    EdgeProp(from, m_metric(from,pm[from])), spanningTree);
            }
        }
        return spanningTree;
    }
    
    void findSave(const AMatrix & g) {
        auto spanningTree = getSpanningTree(g);
        findSaveInner(spanningTree);
    }

    void findSaveInner(GraphType & spanningTree) {
        int n = boost::num_vertices(spanningTree);
        if(n == 1) {   
            return;
        }
        typedef typename boost::graph_traits<GraphType>::edge_descriptor Edge; 
        auto eRange = boost::edges(spanningTree);
        assert(eRange.first != eRange.second);
        auto const  & weight_map = boost::get(boost::edge_weight, spanningTree);
        auto maxEl = std::max_element(eRange.first, eRange.second, [&](Edge e, Edge f){
            return boost::get(weight_map, e) < 
                   boost::get(weight_map, f);
        });
        Dist maxDist = boost::get(weight_map, *maxEl);
        std::vector<VertexType> comps(n);
        boost::remove_edge(*maxEl, spanningTree);
        boost::connected_components(spanningTree, &comps[0]);
        int c1 = comps[0];
        int c2 = -1;
        GraphType G1 = spanningTree.create_subgraph();
        GraphType G2 = spanningTree.create_subgraph();
        
        for(int i = 0; i < n; ++i) {
            if(comps[i] == c1) {
                add_vertex(i, G1);
            } else {
                assert(c2 == -1 || comps[i] == c2);
                c2 = comps[i];
                add_vertex(i, G2);
            }
        }
        auto vertices1 = boost::vertices(G1); 
        auto vertices2 = boost::vertices(G2);
        std::for_each(vertices1.first, vertices1.second, [&](VertexType v){
            std::for_each(vertices2.first, vertices2.second, [&](VertexType w){
                m_findSaveMatrtix.set(v, w, maxDist);
            });
        });
        findSaveInner(G1);
        findSaveInner(G2);
    }

    const Metric & m_metric;
    const Voronoi & m_voronoi;
    int N;
    data_structures::MetricBase<Dist> m_findSaveMatrtix;
};

} // steiner_tree
} //local_search
} //paal
