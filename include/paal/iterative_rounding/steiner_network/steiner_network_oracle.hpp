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

#include "paal/iterative_rounding/steiner_network/prune_restrictions_to_tree.hpp"
#include "paal/utils/double_rounding.hpp"
#include "paal/utils/do_nothing_functor.hpp"


namespace paal {
namespace ir {

template <typename Graph, typename Restrictions>
class SteinerNetworkOracle {
    typedef utils::Compare<double> Compare;
public:
    typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;
    typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
    
    typedef boost::bimap<Edge, ColId> EdgeMap;
    typedef std::set<Edge> ResultNetwork;
    typedef std::vector<Vertex> VertexList;

    
    SteinerNetworkOracle(
            const Graph & g, 
            const Restrictions & restrictions, 
            const EdgeMap & edgeMap,
            const ResultNetwork & res,
            const Compare & comp)
             : m_g(g), 
               m_restrictions(restrictions), 
               m_restrictionsVec(pruneRestrictionsToTree(m_restrictions, boost::num_vertices(m_g))),
               m_auxGraph(boost::num_vertices(g)),
               m_edgeMap(edgeMap),
               m_resultNetwork(res),
               m_compare(comp) { 
               }

    bool checkIfSolutionExists() {
        for (auto const & e : m_edgeMap) {
            Vertex u = source(e.left, m_g);
            Vertex v = target(e.left, m_g);
            addEdge(u, v, 1);
        }
        return !findAnyViolatedConstraint();
    }
                           
    template <typename LP>
    bool feasibleSolution(const LP & lp) {
        fillAuxiliaryDigraph(lp);
        return !findMostViolatedConstraint();
    }
    
    template <typename LP>
    void addViolatedConstraint(LP & lp) {
        lp.addRow(LO, m_violatedRestriction);
        
        for (auto const & e : m_edgeMap) {
            const Vertex & u = boost::source(e.left, m_g);
            const Vertex & v = boost::target(e.left, m_g);
                
            if ((m_violatingSet.find(u) != m_violatingSet.end()) !=
                (m_violatingSet.find(v) != m_violatingSet.end())) {
                    lp.addNewRowCoef(e.right);
            }
        }
        
        lp.loadNewRow();
    }

private:
    //TODO make it signed type
    typedef decltype(std::declval<Restrictions>()(0,0)) Dist;

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
        boost::remove_edge_if(utils::ReturnTrueFunctor(), m_auxGraph);
        m_cap = boost::get(boost::edge_capacity, m_auxGraph);
        m_rev = boost::get(boost::edge_reverse, m_auxGraph);
        m_resCap = boost::get(boost::edge_residual_capacity, m_auxGraph);
        
        for (auto const & e : m_edgeMap) {
            ColId colIdx = e.right;
            double colVal = lp.getColPrim(colIdx);

            if(m_compare.g(colVal, 0)) {
                Vertex u = source(e.left, m_g);
                Vertex v = target(e.left, m_g);
                addEdge(u, v, colVal);
            }
        }
        for (auto const & e : m_resultNetwork) {
            Vertex u = source(e, m_g);
            Vertex v = target(e, m_g);
            addEdge(u, v, 1);
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
            assert(src_trg.first != src_trg.second);
            if(m_compare.g(checkViolationBiggerThan(src_trg.first, src_trg.second), 0)) {
                return true;
            }
        }
        
        return false;
    }
    
    bool findMostViolatedConstraint() {
        double max = 0;

        for (auto const & src_trg : m_restrictionsVec) {
            assert(src_trg.first != src_trg.second);
            max = std::max(checkViolationBiggerThan(src_trg.first, src_trg.second), max);
        }
        
        return m_compare.g(max, 0);
    }
    
    double checkViolationBiggerThan(Vertex  src, Vertex trg, double maximumViolation = 0) {
        double minCut = boost::boykov_kolmogorov_max_flow(m_auxGraph, src, trg);
        double restriction = m_restrictions(src, trg);
        double violation = restriction - minCut;
        
        if (m_compare.g(violation, maximumViolation)) {
            m_violatedRestriction = restriction;
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
    
    const Restrictions & m_restrictions;
    RestrictionsVector m_restrictionsVec;
    
    AuxGraph    m_auxGraph;
    
    
    ViolatingSet        m_violatingSet;
    Dist                m_violatedRestriction;
    
    const EdgeMap       &  m_edgeMap;
    const ResultNetwork &  m_resultNetwork;
    const utils::Compare<double> & m_compare;
    
    boost::property_map < AuxGraph, boost::edge_capacity_t >::type              m_cap;
    boost::property_map < AuxGraph, boost::edge_reverse_t >::type               m_rev;
    boost::property_map < AuxGraph, boost::edge_residual_capacity_t >::type     m_resCap;
};

} //ir
} //paal
#endif /* STEINER_NETWORK_ORACLE_HPP */
