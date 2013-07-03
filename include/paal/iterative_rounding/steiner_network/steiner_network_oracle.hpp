/**
 * @file steiner_network_oracle.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-06-24
 */
#ifndef STEINER_NETWORK_ORACLE_HPP
#define STEINER_NETWORK_ORACLE_HPP

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/boykov_kolmogorov_max_flow.hpp>
#include <boost/bimap.hpp>

#include "paal/utils/double_rounding.hpp"


namespace paal {
namespace ir {

template <typename Graph, typename Restrictions>
class SteinerNetworkOracle {
    typedef utils::Compare<double> Compare;
public:
    typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;
    typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
    
    typedef boost::bimap<Edge, ColId> EdgeMap;
    typedef std::vector<Vertex> VertexList;
    
    SteinerNetworkOracle(
            const Graph & g, 
            const Restrictions & restrictions, 
            const EdgeMap & edgeMap,
            const Compare & comp)
             : m_g(g), 
               m_restrictionsVec(boost::num_vertices(m_g)),
               m_restrictions(restrictions), 
               m_edgeMap(edgeMap),
               m_compare(comp) { 
        fillRestrictions();
    }
                           
    template <typename LP>
    bool feasibleSolution(const LP & lp) {
        fillAuxiliaryDigraph(lp);
        return !findAnyViolatedConstraint();
    }
    
    template <typename LP>
    void addViolatedConstraint(LP & lp) {
        lp.addRow(UP, 0, m_violatingSet.size() - 1);
        
        for (auto const & e : m_edgeMap) {
            const Vertex & u = boost::source(e.left, m_g);
            const Vertex & v = boost::target(e.left, m_g);
            
            if (m_violatingSet.find(u) != m_violatingSet.end() && 
                m_violatingSet.find(v) != m_violatingSet.end()) {
                    lp.addNewRowCoef(e.right);
            }
        }
        
        lp.loadNewRow();
    }

private:
    //TODO make it signed type
    typedef decltype(std::declval<Restrictions>()(0,0)) Dist;
    typedef std::vector<std::pair<Vertex, Vertex>> RestrictionsVector;

    template <typename G>
    struct Mapping { 
        typedef Mapping value_type; 

        Mapping(const G & g, RestrictionsVector & resVec) : 
            m_restrictionsVec(&resVec), m_g(&g) {}

        Mapping & operator=(const Mapping &) = default;
        Mapping & operator=(Mapping &&) = default;
        
        Mapping(const Mapping &) = default;
        Mapping(Mapping &&) = default;

        Mapping & operator*() {
            return *this;
        }

        Mapping & operator++() { 
            return *this;
        }
        
        Mapping & operator++(int) {
            return *this;
        }

        std::pair<Vertex, Vertex> & operator=(Edge e) {
            m_restrictionsVec->push_back(std::make_pair(boost::source(e, *m_g), boost::target(e, *m_g)));
            return m_restrictionsVec->back();
        }
        
        RestrictionsVector * m_restrictionsVec; 
        const G * m_g;
    };

    void fillRestrictions() {
        typedef  boost::property < boost::edge_weight_t, Dist> EdgeProp;
        typedef boost::adjacency_list < boost::vecS, boost::vecS, boost::undirectedS,
                            boost::no_property,  EdgeProp> TGraph;
        int N = boost::num_vertices(m_g);
        TGraph g(N);
        for(int i : boost::irange(0, N)) {
            for(int j : boost::irange(i, N)) {
                boost::add_edge(i, j, 
                        EdgeProp(-std::max(m_restrictions(i, j), m_restrictions(j, i))),  g);
            }
        }
            
        boost::kruskal_minimum_spanning_tree(g, Mapping<TGraph>(g, m_restrictionsVec));
    }


    typedef boost::adjacency_list_traits < boost::vecS, boost::vecS, boost::directedS > Traits;
    typedef Traits::edge_descriptor AuxEdge;
    typedef Traits::vertex_descriptor AuxVertex;
    typedef boost::adjacency_list < boost::vecS, boost::vecS, boost::directedS,
                                    boost::property < boost::vertex_color_t, boost::default_color_type,
                                        boost::property < boost::vertex_distance_t, long,
                                            boost::property < boost::vertex_predecessor_t, AuxEdge >
                                                        >
                                                    >,
                                    boost::property < boost::edge_capacity_t, double,
                                        boost::property < boost::edge_residual_capacity_t, double,
                                            boost::property < boost::edge_reverse_t, AuxEdge >
                                                        >
                                                    >
                                  > AuxGraph;
    typedef std::vector < AuxEdge > AuxEdgeList;
    typedef std::set < AuxVertex > ViolatingSet;
                                  
    template <typename LP>
    void fillAuxiliaryDigraph(const LP & lp) {
        m_auxGraph.clear();
        m_cap = boost::get(boost::edge_capacity, m_auxGraph);
        m_rev = boost::get(boost::edge_reverse, m_auxGraph);
        m_resCap = boost::get(boost::edge_residual_capacity, m_auxGraph);
        
        for (auto const & e : m_edgeMap) {
            ColId colIdx = e.right;
            double colVal = lp.getColPrim(colIdx);
            
            if (!m_compare.e(colVal, 0)) {
                Vertex u = source(e.left, m_g);
                Vertex v = target(e.left, m_g);
                addEdge(u, v, colVal);
            }
        }
    }
    
    template <typename SrcVertex, typename TrgVertex>
    AuxEdge addEdge(const SrcVertex & vSrc, const TrgVertex & vTrg, double cap) {
        bool b, bRev;
        AuxEdge e, eRev;
        
        std::tie(e, b) = boost::add_edge(vSrc, vTrg, m_auxGraph);
        std::tie(eRev, bRev) = boost::add_edge(vTrg, vSrc, m_auxGraph);
        
        assert(b && bRev);
        
        m_cap[e] = cap;
        m_cap[eRev] = cap;
        
        m_rev[e] = eRev;
        m_rev[eRev] = e;
        
        return e;
    }
    
    bool findAnyViolatedConstraint() {
        // TODO random source node
        
        for (auto const & src_trg : m_restrictionsVec) {
            if(m_compare.g(checkViolationBiggerThan(src_trg.first, src_trg.second), 0)) {
                return true;
            }
        }
        
        return false;
    }
    
/*    bool findMostViolatedConstraint() {
        auto vertices = boost::vertices(m_auxGraph);
        const Vertex & src = *(vertices.first);
        assert(src != m_src && src != m_trg);
        
        m_violatedConstraintFound = false;
        m_maximumViolation = 0;
        
        for (const Vertex & trg : utils::make_range(vertices.first, vertices.second)) {
            if (src != trg && trg != m_src && trg != m_trg) {
                checkViolation(src, trg);
                checkViolation(trg, src);
            }
        }
        
        return m_violatedConstraintFound;
    }
*/    
    double checkViolationBiggerThan(Vertex  src, Vertex trg, double maximumViolation = 0) {
        double minCut = boost::boykov_kolmogorov_max_flow(m_auxGraph, src, trg);
        double violation = m_restrictions(src, trg) - minCut;
        
        if (m_compare.g(violation, maximumViolation)) {
            m_violatingSet.clear();
            maximumViolation = violation;
            
            auto vertices = boost::vertices(m_auxGraph);
            auto colors = boost::get(boost::vertex_color, m_auxGraph);
            auto srcColor = boost::get(colors, src);
            for (const Vertex & v : utils::make_range(vertices.first, vertices.second)) {
                if (boost::get(colors, v) == srcColor) {
                    m_violatingSet.insert(v);
                }
            }
        }

        return violation;
        
    }
    
    const Graph &  m_g;
    
    RestrictionsVector m_restrictionsVec;
    const Restrictions & m_restrictions;
    
    AuxGraph    m_auxGraph;
    
    
    ViolatingSet        m_violatingSet;
    
    EdgeMap         m_edgeMap;
    const utils::Compare<double> & m_compare;
    
    boost::property_map < AuxGraph, boost::edge_capacity_t >::type              m_cap;
    boost::property_map < AuxGraph, boost::edge_reverse_t >::type               m_rev;
    boost::property_map < AuxGraph, boost::edge_residual_capacity_t >::type     m_resCap;
};

} //ir
} //paal
#endif /* STEINER_NETWORK_ORACLE_HPP */
