/**
 * @file capacitated_voronoi.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-03-20
 */
#ifndef CAPACITATED_VORONOI_HPP
#define CAPACITATED_VORONOI_HPP 

#include <boost/graph/adjacency_list.hpp>

#include "paal/data_structures/metric/metric_traits.hpp"
#include "paal/min_cost_max_flow/min_cost_max_flow.hpp"
#include "paal/utils/iterator_utils.hpp"


namespace paal {
namespace data_structures {

template <typename Metric, typename GeneratorsCapacieties, typename VerticesDemands>
class CapacitatedVoronoi {
public:
    class Dist {
    public:
        typedef typename MetricTraits<Metric>::DistanceType DistI;
        Dist() {} 
        Dist(DistI real, DistI distToFullAssign) : 
            m_realDist(real), m_distToFullAssignment(distToFullAssign) {}
        Dist operator-(Dist d) {
            return Dist(m_realDist - d.m_realDist, 
                        m_distToFullAssignment - d.m_distToFullAssignment);
        }
        
        DistI getDistToFullAssignment() const {
            return m_distToFullAssignment;
        }

        DistI getRealDist() const {
            return m_realDist;
        }

        bool operator==(Dist d) const {
            return m_realDist ==  d.m_realDist &&
                   m_distToFullAssignment == d.m_distToFullAssignment;
        }
        
        bool operator>(DistI d) {
            if(m_distToFullAssignment > 0) {
                return true;
            } else  if(m_distToFullAssignment < 0) {
                return false;
            }
            return m_realDist > d;
        }
        
        const Dist & operator+=(Dist d) {
            m_realDist += d.m_realDist;
            m_distToFullAssignment += d.m_distToFullAssignment;
            return *this;
        }
        
        Dist operator+(Dist d) {
            Dist ret(d);
            ret += *this;
            return ret;
        }
        
        Dist operator-() {
            return Dist(-m_realDist, m_distToFullAssignment);
        }
    
        friend  Dist  operator+(DistI di, Dist d) {
            return Dist(d.m_realDist + di, d.m_distToFullAssignment); 
        }

        template <typename Stream >
        friend Stream & operator<<(Stream & s, Dist d) {
            return s << d.m_distToFullAssignment << " " <<  d.m_realDist; 
            
        }

    private:
        DistI m_realDist;
        DistI m_distToFullAssignment;
    };
    typedef typename Dist::DistI DistI;
    typedef typename MetricTraits<Metric>::VertexType VertexType;    
    typedef std::set<VertexType> Generators;
    typedef std::vector<VertexType> Vertices;

    CapacitatedVoronoi(const Generators & gen, Vertices ver,
                       const Metric & m, 
                       const GeneratorsCapacieties & gc, const VerticesDemands & vd, 
                       DistI costOfNoGenerator = std::numeric_limits<DistI>::max()/*,
                       DistI infiniteCapacity = std::numeric_limits<DistI>::max()*/) : 
                         m_s(addVertex()), m_t(addVertex()),
                         m_vertices(std::move(ver)), m_metric(m), 
                         m_generatorsCap(gc),
                         m_capacitySum(std::accumulate(m_vertices.begin(), m_vertices.end(), 
                                    DistI(0), [&](DistI d, VertexType v){return d + vd(v);})),
                         m_firstGeneratorId(m_vertices.size() + 2),
                         m_costOfNoGenerator(costOfNoGenerator) 
                            {
        for(VertexType v : m_vertices) {
            VD vGraph = addVertex(v);
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
        VD genGraph = addVertex(gen);
        m_gToGraphV.insert(std::make_pair(gen, genGraph));
        for(const std::pair<VertexType, VD> & v : m_vToGraphV) {
            addEdge(v.second, genGraph, m_metric(v.first, gen), std::numeric_limits<DistI>::max());
        }

        addEdge(genGraph, m_t, 0, m_generatorsCap(gen));
        if(costStart.getDistToFullAssignment() < 0) {
            boost::edmonds_karp_max_flow(m_g, m_s, m_t);
        }

        EI i, end;
        std::tie(i,end) = boost::edges(m_g);

        std::cout << "Stan przed cancel " <<std::endl ;
        auto residual_capacity = boost::get(boost::edge_residual_capacity, m_g);
        for(;i!= end; ++i) {
            std::cout << *i << " " << residual_capacity[*i] <<std::endl ;
        }

        boost::cycle_cancelation(m_g);

        return getCost() - costStart;
    }
        
    // returns diff between new cost and old cost
    Dist remGenerator(VertexType gen) {
        Dist costStart = getCost();
        m_generators.erase(gen);
        auto genGraph = m_gToGraphV[gen];
        auto rev = get(boost::edge_reverse, m_g);
        auto residual_capacity = boost::get(boost::edge_residual_capacity, m_g);
        
        //removing flow from the net
        for(const ED & e : utils::make_range(boost::in_edges(genGraph, m_g))) {
            bool b;
            VD v = boost::source(e, m_g);
            if(v == m_t) {
                continue;
            }
            DistI cap = residual_capacity[rev[e]];
            ED edgeFromStart;
            std::tie(edgeFromStart, b) =  boost::edge(m_s, v, m_g);
            assert(b);
            residual_capacity[edgeFromStart] += cap;
            residual_capacity[rev[edgeFromStart]] -= cap;
        }
        boost::clear_vertex(genGraph, m_g);
        boost::remove_vertex(genGraph, m_g);
        m_generators.erase(gen);
        restoreIndex();
        
        //boost::path_augmentation_from_residual(gRes, m_s, m_t);
        boost::edmonds_karp_max_flow(m_g, m_s, m_t);
        boost::cycle_cancelation(m_g);

        return getCost() - costStart;
    }
    
    const Generators & getGenerators() const {
        return m_generators;
    }
    
    const Vertices & getVertices() const {
        return m_vertices;
    }


    Dist getCost() const  {
        auto residual_capacity = boost::get(boost::edge_residual_capacity, m_g);
        OEI ei, end;
        std::tie(ei, end) = boost::out_edges(m_s, m_g);
        DistI resCap =  std::accumulate(ei, end, DistI(0), [&](DistI d, const ED & e) {
            return d + residual_capacity[e];
        });

        DistI cost =  boost::find_min_cost(m_g);
        return Dist(cost, -resCap);
    }
    
private:



    void restoreIndex() {
        unsigned N = boost::num_vertices(m_g);
        m_gToGraphV.clear();
        auto name = boost::get(boost::vertex_name, m_g);
        for(unsigned i : boost::irange(m_firstGeneratorId, N)) {
            m_gToGraphV[name[i]] = i;
        }
    }

    typedef boost::adjacency_list < boost::listS, boost::vecS,  boost::bidirectionalS,
        boost::property<boost::vertex_name_t, VertexType >,
            boost::property < boost::edge_capacity_t, DistI,
                boost::property < boost::edge_residual_capacity_t, DistI,
                    boost::property < boost::edge_reverse_t, 
                                        boost::adjacency_list_traits <boost::listS, boost::vecS, boost::bidirectionalS >::edge_descriptor, 
                      boost::property <boost::edge_weight_t, DistI>
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
    
    VD addVertex(VertexType v = VertexType()) {
//        auto rev = get(boost::edge_reverse, m_g);
        VD vG = boost::add_vertex(boost::property<boost::vertex_name_t, VertexType>(v), m_g);
  //      bool b;
    //    ED e;
        /*std::tie(e,b) = boost::add_edge(vG, vG, m_g);
        assert(b);
        rev[e] = e;*/
        return vG;
    }


    void addEdge(VD v, VD w, DistI weight, DistI capacity) {
        auto rev = get(boost::edge_reverse, m_g);
        ED e,f;
        e = addDirEdge(v, w, weight, capacity);
        f = addDirEdge(w, v, -weight, 0);
        rev[e] = f; 
        rev[f] = e; 
    }
    
    ED addDirEdge(VD v, VD w, DistI weight, DistI capacity) {
        bool b;
        ED e;
        auto weightMap = boost::get(boost::edge_weight, m_g);
        auto capacityMap = boost::get(boost::edge_capacity, m_g);
        auto residual_capacity = boost::get(boost::edge_residual_capacity, m_g);
        std::tie(e, b) = add_edge(v, w, m_g);
        assert(b);
        capacityMap[e] = capacity;
        residual_capacity[e] = capacity;
        weightMap[e] = weight;
        return e;
    }
    
    DistI getFlowOnEdge(const ED & e) const {
        auto capacityMap = boost::get(boost::edge_capacity, m_g);
        auto residual_capacity = boost::get(boost::edge_residual_capacity, m_g);
        return capacityMap[e] - residual_capacity[e];
    }
    
    VertexType getVertexForEdge(const ED & e) const  {
        auto name = boost::get(boost::vertex_name, m_g);
        return name[boost::source(e, m_g)];
    }

    struct Trans {
        std::pair<VertexType, DistI> operator()(const ED &e) const {
            return std::make_pair(m_v->getVertexForEdge(e), m_v->getFlowOnEdge(e));
        }
        const CapacitatedVoronoi * m_v;
    };

    typedef boost::transform_iterator<Trans, IEI, std::pair<VertexType, DistI>> VForGenerator;
public:
    
    /**
     * @brief member function for getting assignment, for generator.
     *
     * @return returns range of pairs; the first element of pair is the Vertex 
     * and the second element is the flow from this vertext to given generator
     *
     */
    std::pair<VForGenerator, VForGenerator> 
    getVerticesForGenerator(VertexType gen) const {
        IEI ei, end;
        VD v = m_gToGraphV.at(gen);
        auto r = boost::in_edges(v, m_g);
        Trans t; 
        t.m_v = this;
        return std::make_pair(VForGenerator(r.first,  t),
                              VForGenerator(r.second, t));
    }

private:

    Graph m_g;
    VD m_s,m_t;

    Generators m_generators;
    Vertices m_vertices;
    const Metric & m_metric;
    const GeneratorsCapacieties & m_generatorsCap;
    const DistI m_capacitySum;
    const VD m_firstGeneratorId;
    DistI m_costOfNoGenerator;
    VertexToGraphVertex m_vToGraphV;
    VertexToGraphVertex m_gToGraphV;
};


} // data_structures
} //paal
#endif /* CAPACITATED_VORONOI_HPP */
