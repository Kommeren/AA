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
#include "paal/utils/functors.hpp"


namespace paal {
namespace ir {

template <typename Graph, typename Restrictions, typename ResultNetworkSet>
class SteinerNetworkOracle {
    typedef utils::Compare<double> Compare;
public:
    typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;
    typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
    
    
    SteinerNetworkOracle(
            const Graph & g, 
            const Restrictions & restrictions, 
            const ResultNetworkSet & res)
             : m_g(g), 
               m_restrictions(restrictions), 
               m_restrictionsVec(pruneRestrictionsToTree(m_restrictions, num_vertices(m_g))),
               m_auxGraph(num_vertices(g)),
               m_resultNetwork(res)
               {}

    template <typename Problem>
    bool checkIfSolutionExists(Problem & problem) {
        for (auto const & e : problem.getEdgeMap()) {
            Vertex u = source(e.left, m_g);
            Vertex v = target(e.left, m_g);
            addEdge(u, v, 1);
        }
        return !findAnyViolatedConstraint(problem);
    }
                           
    template <typename Problem, typename LP>
    bool feasibleSolution(Problem & problem, const LP & lp) {
        fillAuxiliaryDigraph(problem, lp);
        return !findMostViolatedConstraint(problem);
    }
    
    template <typename Problem, typename LP>
    void addViolatedConstraint(Problem & problem, LP & lp) {
        lp.addRow(LO, m_violatedRestriction);
        
        for (auto const & e : problem.getEdgeMap()) {
            const Vertex & u = source(e.left, m_g);
            const Vertex & v = target(e.left, m_g);
                
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
                                  
    template <typename Problem, typename LP>
    void fillAuxiliaryDigraph(Problem & problem, const LP & lp) {
        remove_edge_if(utils::ReturnTrueFunctor(), m_auxGraph);
        m_cap = get(boost::edge_capacity, m_auxGraph);
        m_rev = get(boost::edge_reverse, m_auxGraph);
        m_resCap = get(boost::edge_residual_capacity, m_auxGraph);
        
        for (auto const & e : problem.getEdgeMap()) {
            ColId colIdx = e.right;
            double colVal = lp.getColPrim(colIdx);

            if (problem.getCompare().g(colVal, 0)) {
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
        
        std::tie(e, b) = add_edge(vSrc, vTrg, m_auxGraph);
        std::tie(eRev, bRev) = add_edge(vTrg, vSrc, m_auxGraph);
        
        assert(b && bRev);
        
        m_cap[e] = cap;
        m_cap[eRev] = cap;
        
        m_rev[e] = eRev;
        m_rev[eRev] = e;
        
        return e;
    }
   
    template <typename Problem>
    bool findAnyViolatedConstraint(Problem & problem) {
        // TODO random source node
        
        for (auto const & src_trg : m_restrictionsVec) {
            assert(src_trg.first != src_trg.second);
            if(problem.getCompare().g(checkViolationBiggerThan(problem, src_trg.first, src_trg.second), 0)) {
                return true;
            }
        }
        
        return false;
    }
    
    template <typename Problem>
    bool findMostViolatedConstraint(Problem & problem) {
        double max = 0;

        for (auto const & src_trg : m_restrictionsVec) {
            assert(src_trg.first != src_trg.second);
            max = std::max(checkViolationBiggerThan(problem, src_trg.first, src_trg.second), max);
        }
        
        return problem.getCompare().g(max, 0);
    }
    
    template <typename Problem>
    double checkViolationBiggerThan(Problem & problem, Vertex  src, Vertex trg, double maximumViolation = 0) {
        double minCut = boykov_kolmogorov_max_flow(m_auxGraph, src, trg);
        double restriction = m_restrictions(src, trg);
        double violation = restriction - minCut;
        
        if (problem.getCompare().g(violation, maximumViolation)) {
            m_violatedRestriction = restriction;
            m_violatingSet.clear();
            maximumViolation = violation;
            
            auto colors = get(boost::vertex_color, m_auxGraph);
            auto srcColor = get(colors, src);
            for (const Vertex & v : boost::make_iterator_range(vertices(m_auxGraph))) {
                if (get(colors, v) == srcColor) {
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
    
    const ResultNetworkSet &  m_resultNetwork;
    
    boost::property_map < AuxGraph, boost::edge_capacity_t >::type              m_cap;
    boost::property_map < AuxGraph, boost::edge_reverse_t >::type               m_rev;
    boost::property_map < AuxGraph, boost::edge_residual_capacity_t >::type     m_resCap;
};

template <typename Graph, typename Restrictions, typename ResultNetworkSet>
SteinerNetworkOracle<Graph, Restrictions, ResultNetworkSet>
make_SteinerNetworkSeparationOracle(const Graph & g, const Restrictions & restrictions, const ResultNetworkSet & res) {
    return SteinerNetworkOracle<Graph, Restrictions, ResultNetworkSet>(g, restrictions, res);
}


} //ir
} //paal
#endif /* STEINER_NETWORK_ORACLE_HPP */
