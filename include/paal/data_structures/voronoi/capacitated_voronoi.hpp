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

template <typename Metric, typename VerticesGeneratorsCapacities, typename GeneratorsCapacieties, typename VerticesDemands>
class CapacitatedVoronoi {
public:
    typedef typename MetricTraits<Metric>::VertexType VertexType;    
    typedef std::set<VertexType> Generators;
    typedef typename MetricTraits<Metric>::DistanceType Dist;
    typedef std::vector<int> Vertices;

    CapacitatedVoronoi(const Generators & gen, Vertices ver,
                       const Metric & m, const VerticesGeneratorsCapacities & vgc, 
                       const GeneratorsCapacieties & gc, const VerticesDemands & vd, 
                       Dist costOfNoGenerator = std::numeric_limits<Dist>::max() ) : 
                         m_residual_capacity(boost::get(boost::edge_residual_capacity, m_g)),
                         m_capacity(boost::get(boost::edge_capacity, m_g)),
                         m_weight(boost::get(boost::edge_weight, m_g)),
                         m_rev(get(boost::edge_reverse, m_g)),
                         m_gRes(boost::detail::residual_graph(m_g, m_residual_capacity)),
                         m_s(boost::add_vertex(m_g)), m_t(boost::add_vertex(m_g)),
                         m_vertices(std::move(ver)), m_metric(m), m_verticesGeneratorsCap(vgc), 
                         m_generatorsCap(gc),
                         m_costOfNoGenerator(costOfNoGenerator) {
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
        VD genGraph = add_vertex(m_g);
        m_gToGraphV.insert(std::make_pair(gen, genGraph));
        for(const std::pair<VertexType, VD> & v : m_vToGraphV) {
            addEdge(genGraph, v.second, m_metric(v.first, gen), m_verticesGeneratorsCap(v.first, gen));
        }
        
        

        if(m_generators.size() == 1) {
        } else {
        }
        return getCost() - costStart;
    }
        
    // returns diff between new cost and old cost
    Dist remGenerator(VertexType gen) {
        Dist costStart = getCost();
        m_generators.erase(gen);
        auto v = m_vToGraphV.find(gen);
        boost::clear_vertex(v->second, m_g);
        boost::remove_vertex(v->second, m_g);
        m_generators.erase(gen);
        restoreIndex();
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
    }

    Dist getCost() {
        return 0;    
    }
    
    typedef boost::adjacency_list < boost::listS, boost::vecS, boost::directedS,
        boost::property<boost::vertex_name_t, int >,
            boost::property < boost::edge_capacity_t, Dist,
                boost::property < boost::edge_residual_capacity_t, Dist,
                    boost::property < boost::edge_reverse_t, 
                                        boost::adjacency_list_traits <boost::vecS, boost::vecS, boost::directedS >::edge_descriptor, 
                      boost::property <boost::edge_weight_t, Dist>
                             > 
                          > 
                      > > Graph;
    typedef boost::graph_traits<Graph> GTraits;
    typedef typename GTraits::edge_descriptor ED;
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
    const VerticesGeneratorsCapacities & m_verticesGeneratorsCap;
    const GeneratorsCapacieties & m_generatorsCap;
    Dist m_costOfNoGenerator;
    VertexToGraphVertex m_vToGraphV;
    VertexToGraphVertex m_gToGraphV;
};

} // data_structures
} //paal
#endif /* CAPACITATED_VORONOI_HPP */
