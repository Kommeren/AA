/**
 * @file steiner_tree_oracle.hpp
 * @brief 
 * @author Maciej Andrejczuk
 * @version 1.0
 * @date 2013-08-01
 */
#ifndef STEINER_TREE_ORACLE_HPP
#define STEINER_TREE_ORACLE_HPP

#include <unordered_map>
#include <unordered_set>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/boykov_kolmogorov_max_flow.hpp>

#include "paal/iterative_rounding/steiner_tree/steiner_components.hpp"

namespace paal {
namespace ir {

/**
 * Separation Oracle class.
 */
template<typename Vertex, typename Dist, typename Terminals> class SteinerTreeOracle {
public:
    SteinerTreeOracle() :
        m_terminals(NULL), m_components(NULL), m_currentGraphSize(-1) {}

    /**
     * Finds a solution that satisfies all current constrains.
     */
    template <typename Solution, typename LP>
    bool feasibleSolution(Solution & sol, const LP & lp) {
        int graphSize = sol.getTerminals().size();
        if (graphSize != m_currentGraphSize) {
            // Graph has changed, construct new oracle
            m_currentGraphSize = graphSize;
            m_components = &sol.getComponents();
            m_terminals = &sol.getTerminals();
            m_root = selectRoot();
            createAuxiliaryDigraph(sol, lp);
        } else {
            updateAuxiliaryDigraph(sol, lp);
        }
        //return !findMostViolatedConstraint();
        return !findAnyViolatedConstraint(sol);
    }

    /**
     * Adds the violated constraint to LP.
     * It contains all the components reachable from a given source,
     * but its sink vertex is not reachable.
     */
    template <typename Solution, typename LP>
    void addViolatedConstraint(Solution & sol, LP & lp) {
        lp.addRow(LO, 1);
        for (int i = 0; i < m_components->size(); ++i) {
            Vertex u = m_artifVertices[i];
            int ver = m_components->findVersion(i);
            Vertex v = m_components->find(i).getSink(ver);
            if (m_violatingSet.find(u) != m_violatingSet.end()
                   && m_violatingSet.find(v) == m_violatingSet.end()) {
                ColId colIdx = sol.findColumnLP(i);
                lp.addNewRowCoef(colIdx);
            }
        }
        lp.loadNewRow();
    }

private:
    typedef boost::adjacency_list_traits<boost::vecS, boost::vecS,
            boost::directedS> Traits;
    typedef Traits::edge_descriptor AuxEdge;
    typedef Traits::vertex_descriptor AuxVertex;
    typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS,
            boost::property<boost::vertex_color_t, boost::default_color_type,
                    boost::property<boost::vertex_distance_t, long,
                            boost::property<boost::vertex_predecessor_t, AuxEdge> > >,
            boost::property<boost::edge_capacity_t, double,
                    boost::property<boost::edge_residual_capacity_t, double,
                            boost::property<boost::edge_reverse_t, AuxEdge> > > > AuxGraph;
    typedef std::vector<AuxEdge> AuxEdgeList;

    /**
     * @brief Creates the auxiliary directed graph used for feasibility testing
     *
     * Graph contains a vertex for each component and each terminal
     * Sources of every component have out edges with infinite weight
     * Target has in edge with weigth x_i from LP
     */
    template <typename Solution, typename LP>
    void createAuxiliaryDigraph(Solution &sol, const LP & lp) {
        m_auxGraph.clear();
        m_artifVertices.clear();
        m_cap = get(boost::edge_capacity, m_auxGraph);
        m_rev = get(boost::edge_reverse, m_auxGraph);
        m_resCap = get(boost::edge_residual_capacity, m_auxGraph);

        for (int i = 0; i < (int)m_terminals->size(); ++i) {
            add_vertex(m_auxGraph);
        }

        for (int i = 0; i < m_components->size(); ++i) {
            AuxVertex newV = add_vertex(m_auxGraph);
            m_artifVertices[i] = newV;
            int ver = m_components->findVersion(i);
            Vertex sink = m_components->find(i).getSink(ver);
            for (Vertex w : boost::make_iterator_range(m_components->find(i).getElements())) {
                if (w != sink) {
                    double INF = std::numeric_limits<double>::max();
                    addEdge(w, newV, INF);
                } else {
                    ColId x = sol.findColumnLP(i);
                    double colVal = lp.getColPrim(x);
                    addEdge(newV, sink, colVal);
                }
            }
        }
    }

    /**
     * Updates the auxiliary directed graph. Should be performed after each LP iteration.
     */
    template <typename Solution, typename LP>
    void updateAuxiliaryDigraph(Solution &sol, const LP & lp) {
        for (int i = 0; i < m_components->size(); ++i) {
            AuxVertex componentV = m_artifVertices[i];
            int ver = m_components->findVersion(i);
            Vertex sink = m_components->find(i).getSink(ver);
            ColId x = sol.findColumnLP(i);
            double colVal = lp.getColPrim(x);
            addEdge(componentV, sink, colVal);
        }
    }

    /**
     * Select the root terminal. Max-flow will be directed to that vertex during LP oracle execution.
     */
    Vertex selectRoot() {
        //TODO: Maybe it's better to select random vertex rather than first
        Vertex ret = *m_terminals->begin();
        return ret;
    }

    /**
     * @brief Adds an edge to the auxiliary graph
     * @param vSrc source vertex of for the added edge
     * @param vTrg target vertex of for the added edge
     * @param cap capacity of the added edge
     * @return created edge of the auxiliary graph
     *
     * @tparam SrcVertex
     * @tparam TrgVertex
     */
    template<typename SrcVertex, typename TrgVertex> AuxEdge addEdge(
            const SrcVertex & vSrc, const TrgVertex & vTrg, double cap) {
        bool b, bRev;
        AuxEdge e, eRev;

        std::tie(e, b) = add_edge(vSrc, vTrg, m_auxGraph);
        std::tie(eRev, bRev) = add_edge(vTrg, vSrc, m_auxGraph);
        assert(b && bRev);

        if (cap < 0) {
            assert(cap > -1e-7);
            cap = 0;
        }

        put(m_cap, e, cap);
        put(m_cap, eRev, 0);
        put(m_rev, e, eRev);
        put(m_rev, eRev, e);
        return e;
    }

    /**
     * Runs a max-flow algorithm from every vertex or until first violation is found.
     */
    template <typename Solution>
    bool findAnyViolatedConstraint(Solution & sol) {
        double violation = 0;
        for (const Vertex & src : boost::make_iterator_range(*m_terminals)) {
            if (src != m_root) {
                violation = checkViolationBiggerThan(sol, src, m_root);
                if (sol.getCompare().g(violation, 0)) {
                    return true;
                }
            }
        }
        return false;
    }

    /**
     *  @brief Runs a max-flow from every vertex towards root.
     */
    template <typename Solution>
    bool findMostViolatedConstraint(Solution & sol) {
        double max = 0;
        for (const Vertex & src : boost::make_iterator_range(*m_terminals)) {
            if (src == m_root)
                continue;
            max = std::max(checkViolationBiggerThan(sol, src, m_root, max), max);
        }
        return sol.getCompare().g(max, 0);
    }

    /**
     * Runs a maxflow algorithm between given vertices.
     */
    template <typename Solution>
    double checkViolationBiggerThan(Solution & sol, Vertex  src, Vertex trg, double maximumViolation = 0) {
        double minCut = boykov_kolmogorov_max_flow(m_auxGraph, src, trg);
        double violation = 1 - minCut;
        if (sol.getCompare().g(violation, maximumViolation)) {
            maximumViolation = violation;
            findViolatingSet(src, trg);
        }
        return violation;
    }

    /**
     * Finds a set of vertices unreachable from source.
     */
    void findViolatingSet(Vertex src, Vertex trg) {
        m_violatingSet.clear();
        auto colors = get(boost::vertex_color, m_auxGraph);
        auto srcColor = get(colors, src);
        for (const AuxVertex & v : boost::make_iterator_range(vertices(m_auxGraph))) {
            if (get(colors, v) == srcColor) {
                m_violatingSet.insert(v);
            }
        }
    }

    Terminals const * m_terminals; // current list of terminals
    SteinerComponents<Vertex, Dist> const * m_components; // current list of components
    Vertex m_root; // root vertex, sink of all max-flows
    int m_currentGraphSize; // size of current graph

    AuxGraph m_auxGraph; // max-flow graph
    std::unordered_map<int, AuxVertex> m_artifVertices; // maps componentId to auxGraph vertex
    std::unordered_set<AuxVertex> m_violatingSet; // set of unreachable vertices

    boost::property_map<AuxGraph, boost::edge_capacity_t>::type m_cap; // capacity
    boost::property_map<AuxGraph, boost::edge_reverse_t>::type m_rev; // reverse edge
    boost::property_map<AuxGraph, boost::edge_residual_capacity_t>::type m_resCap; // residual capacity
};

} //ir
} //paal
#endif /* STEINER_TREE_ORACLE_HPP */
