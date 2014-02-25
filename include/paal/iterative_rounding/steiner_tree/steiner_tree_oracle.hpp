/**
 * @file steiner_tree_oracle.hpp
 * @brief
 * @author Maciej Andrejczuk, Piotr Godlewski
 * @version 1.0
 * @date 2013-08-01
 */
#ifndef STEINER_TREE_ORACLE_HPP
#define STEINER_TREE_ORACLE_HPP

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/boykov_kolmogorov_max_flow.hpp>
#include <boost/optional.hpp>

namespace paal {
namespace ir {

/**
 * @class SteinerTreeViolationChecker
 * @brief Violations checker for the separation oracle
 *      in the steiner tree problem.
 */
class SteinerTreeViolationChecker {
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
    typedef std::unordered_set<AuxVertex> ViolatingSet;
    typedef std::vector<ViolatingSet> ViolatingSets;
    typedef boost::optional<double> Violation;

public:
    typedef AuxVertex Candidate;

    SteinerTreeViolationChecker() :
        m_currentGraphSize(-1) {}

    /**
     * Returns an iterator range of violated constraint candidates.
     */
    template <typename Problem, typename LP>
    auto getViolationCandidates(const Problem & problem, const LP & lp)
            -> decltype(problem.getTerminals()) {

        int graphSize = problem.getTerminals().size();
        if (graphSize != m_currentGraphSize) {
            // Graph has changed, construct new oracle
            m_currentGraphSize = graphSize;
            m_root = selectRoot(problem.getTerminals());
            createAuxiliaryDigraph(problem, lp);
        } else {
            updateAuxiliaryDigraph(problem, lp);
        }
        return problem.getTerminals();
    }

    /**
     * Checks if the given constraint candidate is violated an if it is,
     * returns the violation value and violated constraint ID.
     */
    template <typename Problem>
    Violation checkViolation(Candidate candidate, const Problem & problem) {
        if (candidate == m_root) {
            return Violation();
        }

        double violation = checkMinCut(candidate, m_root, problem.getCompare());
        if (problem.getCompare().g(violation, 0)) {
            return Violation(violation);
        }
        else {
            return Violation();
        }
    }

    /**
     * Adds the violated constraint to LP.
     * It contains all the components reachable from a given source,
     * but its sink vertex is not reachable.
     */
    template <typename Problem, typename LP>
    void addViolatedConstraint(Candidate violatingTerminal, const Problem & problem, LP & lp) {
        if (m_violatingTerminal != violatingTerminal) {
            checkMinCut(violatingTerminal, m_root, problem.getCompare());
        }

        const auto & components = problem.getComponents();
        lp.addRow(lp::LO, 1);
        for (int i = 0; i < components.size(); ++i) {
            auto u = m_artifVertices[i];
            int ver = components.findVersion(i);
            auto v = components.find(i).getSink(ver);
            if (m_violatingSet.find(u) != m_violatingSet.end()
                   && m_violatingSet.find(v) == m_violatingSet.end()) {
                lp::ColId colIdx = problem.findColumnLP(i);
                lp.addNewRowCoef(colIdx);
            }
        }
        lp.loadNewRow();
    }

private:

    /**
     * @brief Creates the auxiliary directed graph used for feasibility testing
     *
     * Graph contains a vertex for each component and each terminal
     * Sources of every component have out edges with infinite weight
     * Target has in edge with weigth x_i from LP
     */
    template <typename Problem, typename LP>
    void createAuxiliaryDigraph(Problem &problem, const LP & lp) {
        m_auxGraph.clear();
        m_artifVertices.clear();
        m_cap = get(boost::edge_capacity, m_auxGraph);
        m_rev = get(boost::edge_reverse, m_auxGraph);
        const auto & components = problem.getComponents();

        for (int i = 0; i < (int)problem.getTerminals().size(); ++i) {
            add_vertex(m_auxGraph);
        }

        for (int i = 0; i < components.size(); ++i) {
            AuxVertex newV = add_vertex(m_auxGraph);
            m_artifVertices[i] = newV;
            int ver = components.findVersion(i);
            auto sink = components.find(i).getSink(ver);
            for (auto w : boost::make_iterator_range(components.find(i).getElements())) {
                if (w != sink) {
                    double INF = std::numeric_limits<double>::max();
                    addEdge(w, newV, INF);
                } else {
                    lp::ColId x = problem.findColumnLP(i);
                    double colVal = lp.getColPrim(x);
                    addEdge(newV, sink, colVal);
                }
            }
        }
    }

    /**
     * Updates the auxiliary directed graph. Should be performed after each LP iteration.
     */
    template <typename Problem, typename LP>
    void updateAuxiliaryDigraph(Problem &problem, const LP & lp) {
        const auto & components = problem.getComponents();
        for (int i = 0; i < components.size(); ++i) {
            AuxVertex componentV = m_artifVertices[i];
            int ver = components.findVersion(i);
            auto sink = components.find(i).getSink(ver);
            lp::ColId x = problem.findColumnLP(i);
            double colVal = lp.getColPrim(x);
            addEdge(componentV, sink, colVal);
        }
    }

    /**
     * Select the root terminal. Max-flow will be directed to that vertex during LP oracle execution.
     */
    template <typename Terminals>
    AuxVertex selectRoot(const Terminals & terminals) {
        //TODO: Maybe it's better to select random vertex rather than first
        AuxVertex ret = *terminals.begin();
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
     * Runs a maxflow algorithm between given vertices.
     */
    template <typename Compare>
    double checkMinCut(AuxVertex src, AuxVertex trg, Compare compare) {
        double minCut = boost::boykov_kolmogorov_max_flow(m_auxGraph, src, trg);
        double violation = 1 - minCut;
        if (compare.g(violation, 0)) {
            findViolatingSet(src, trg);
        }
        return violation;
    }

    /**
     * Finds a set of vertices unreachable from source.
     */
    void findViolatingSet(AuxVertex src, AuxVertex trg) {
        m_violatingSet.clear();
        auto colors = get(boost::vertex_color, m_auxGraph);
        auto srcColor = get(colors, src);
        for (AuxVertex v : boost::make_iterator_range(vertices(m_auxGraph))) {
            if (get(colors, v) == srcColor) {
                m_violatingSet.insert(v);
            }
        }
        m_violatingTerminal = src;
    }

    AuxVertex m_root; // root vertex, sink of all max-flows
    int m_currentGraphSize; // size of current graph

    AuxGraph m_auxGraph; // max-flow graph
    std::unordered_map<int, AuxVertex> m_artifVertices; // maps componentId to auxGraph vertex
    ViolatingSet m_violatingSet; // set of unreachable vertices
    Candidate m_violatingTerminal;

    boost::property_map<AuxGraph, boost::edge_capacity_t>::type m_cap; // capacity
    boost::property_map<AuxGraph, boost::edge_reverse_t>::type m_rev; // reverse edge
};

} //ir
} //paal
#endif /* STEINER_TREE_ORACLE_HPP */
