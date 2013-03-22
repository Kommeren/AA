/**
 * @file capacitated_voronoi.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-03-20
 */
#ifndef CAPACITATED_VORONOI_HPP
#define CAPACITATED_VORONOI_HPP 

#include "paal/data_structures/metric/metric_traits.hpp"
#include "paal/min_cost_max_flow/min_cost_max_flow.hpp"
#include <boost/graph/adjacency_list.hpp>

namespace paal {
namespace data_structures {

template <typename Metric, typename GeneratorsCapacieties, typename VerticesDemands>
class CapacitatedVoronoi {
public:
    typedef typename MetricTraits<Metric>::VertexType VertexType;    
    typedef std::set<VertexType> Generators;
    typedef typename MetricTraits<Metric>::DistanceType Dist;
    typedef std::vector<int> Vertices;

    CapacitatedVoronoi(const Generators & gen, Vertices ver,
                       const Metric & m, 
                       const GeneratorsCapacieties & gc, const VerticesDemands & vd, 
                       Dist costOfNoGenerator = std::numeric_limits<Dist>::max() ) : 
                         m_residual_capacity(boost::get(boost::edge_residual_capacity, m_g)),
                         m_capacity(boost::get(boost::edge_capacity, m_g)),
                         m_weight(boost::get(boost::edge_weight, m_g)),
                         m_rev(get(boost::edge_reverse, m_g)),
                         m_gRes(boost::detail::residual_graph(m_g, m_residual_capacity)),
                         m_s(boost::add_vertex(m_g)), m_t(boost::add_vertex(m_g)),
                         m_vertices(std::move(ver)), m_metric(m), 
                         m_generatorsCap(gc),
                         m_capacitySum(std::accumulate(m_vertices.begin(), m_vertices.end(), 
                                    Dist(0), [&](Dist d, VertexType v){return d + vd(v);})),
                         m_firstGeneratorId(m_vertices.size() + 2),
                         m_costOfNoGenerator(costOfNoGenerator) 
                            {
        for(VertexType v : m_vertices) {
            VD vGraph = boost::add_vertex(m_g);
            m_vToGraphV.insert(std::make_pair(v, vGraph));
            addEdge(m_s, vGraph, 0, vd(v));
        }
        for(VertexType g : gen) {
            addGenerator(g);
        }
    }

    // returns diff between new cost and old cost
    Dist addGenerator(VertexType gen) {
        Dist costStart = getCost();
        m_generators.insert(gen);
        VD genGraph = add_vertex(boost::property<boost::vertex_name_t, int>(gen), m_g);
        m_gToGraphV.insert(std::make_pair(gen, genGraph));
        for(const std::pair<VertexType, VD> & v : m_vToGraphV) {
            addEdge(genGraph, v.second, m_metric(v.first, gen), 0);
        }

        addEdge(genGraph, m_t, 0, m_generatorsCap(gen));

        boost::cycle_cancelation_from_residual(m_gRes);

        return getCost() - costStart;
    }
        
    // returns diff between new cost and old cost
    Dist remGenerator(VertexType gen) {
        Dist costStart = getCost();
        m_generators.erase(gen);
        auto genGraph = m_gToGraphV[gen];
        
        
        //removing flow from the net
        for(const ED & e : utils::make_range(boost::in_edges(genGraph, m_g))) {
            VD v = boost::source(e, m_g);
            Dist cap = m_residual_capacity[m_rev[e]];
            ED edgeFromStart = boost::edge(m_s, v, m_g).first;
            m_residual_capacity[edgeFromStart] += cap;
            m_residual_capacity[m_rev[edgeFromStart]] -= cap;
        }
        boost::clear_vertex(genGraph, m_g);
        boost::remove_vertex(genGraph, m_g);
        m_generators.erase(gen);
        restoreIndex();
        
        boost::path_augmentation_from_residual(m_gRes, m_s, m_t);
        boost::cycle_cancelation_from_residual(m_gRes);

        return getCost() - costStart;
    }
    
    const Generators & getGenerators() const {
        return m_generators;
    }
    
    const Vertices & getVertices() const {
        return m_vertices;
    }

    //fake
    std::pair<typename std::vector<int>::iterator, typename std::vector<int>::iterator > 
        getVerticesForGenerator(VertexType gen) const {
            typename std::vector<int>::iterator i;
            return std::make_pair(i, i);
    }

private:
    void addEdge(int v, int w, int weight, int capacity) {
        ED e,f;
        e = addDirEdge(v, w, weight, capacity);
        f = addDirEdge(w, v, -weight, 0);
        m_rev[e] = f; 
        m_rev[f] = e; 
    }

    void restoreIndex() {
        unsigned N = boost::num_vertices(m_g);
        m_gToGraphV.clear();
        auto name = boost::get(boost::vertex_name, m_g);
        for(unsigned i : boost::irange(m_firstGeneratorId, N)) {
            m_gToGraphV[name[i]] = i;
        }
    }

    Dist getCost() {
        OEI ei, end;
        std::tie(ei, end) = boost::out_edges(m_s, m_g);
        Dist resCap =  std::accumulate(ei, end, Dist(0), [&](Dist d, const ED & e){
            return d + m_residual_capacity[e];
        });

        //There are clients that are not fully assigned
        if(resCap > Dist(0)) {
            return m_costOfNoGenerator - (m_capacitySum - resCap);
        } else { //all clients are assigned now we compute the cost of the assignment
            return boost::find_min_cost(m_gRes);
        }
    }
    
    typedef boost::adjacency_list < boost::listS, boost::vecS,  boost::bidirectionalS,
        boost::property<boost::vertex_name_t, int >,
            boost::property < boost::edge_capacity_t, Dist,
                boost::property < boost::edge_residual_capacity_t, Dist,
                    boost::property < boost::edge_reverse_t, 
                                        boost::adjacency_list_traits <boost::listS, boost::vecS, boost::bidirectionalS >::edge_descriptor, 
                      boost::property <boost::edge_weight_t, Dist>
                             > 
                          > 
                      > > Graph;
    typedef boost::graph_traits<Graph> GTraits;
    typedef typename GTraits::edge_descriptor ED;
    typedef typename GTraits::edge_iterator EI;
    typedef typename GTraits::out_edge_iterator OEI;
    typedef typename GTraits::in_edge_iterator IEI;
    typedef typename GTraits::vertex_descriptor VD;
    typedef typename boost::property_map < Graph, boost::edge_capacity_t >::type Capacity;
    typedef typename boost::property_map < Graph, boost::edge_residual_capacity_t >::type ResidualCapacity;
    typedef typename boost::property_map < Graph, boost::edge_weight_t >::type Weight;
    typedef typename boost::property_map < Graph, boost::edge_reverse_t >::type Reversed;
    typedef typename std::map<VertexType, VD> VertexToGraphVertex;
    typedef boost::filtered_graph<Graph, 
        boost::is_residual_edge<ResidualCapacity> > ResidualGraph;
    
    ED addDirEdge(int v, int w, int weight, int capacity) {
        bool b;
        ED e;
        std::tie(e, b) = add_edge(v, w, m_g);
        assert(b);
        m_capacity[e] = capacity;
        m_residual_capacity[e] = capacity;
        m_weight[e] = weight;
        return e;
    }

    Graph m_g;
    ResidualCapacity m_residual_capacity;
    Capacity  m_capacity;
    Weight m_weight;
    Reversed m_rev;
    ResidualGraph m_gRes;
    VD m_s,m_t;


    Generators m_generators;
    Vertices m_vertices;
    const Metric & m_metric;
    const GeneratorsCapacieties & m_generatorsCap;
    const Dist m_capacitySum;
    const VD m_firstGeneratorId;
    Dist m_costOfNoGenerator;
    VertexToGraphVertex m_vToGraphV;
    VertexToGraphVertex m_gToGraphV;
};

} // data_structures
} //paal
#endif /* CAPACITATED_VORONOI_HPP */
