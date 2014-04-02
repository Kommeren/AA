/**
 * @file steiner_tree_oracle.hpp
 * @brief
 * @author Maciej Andrejczuk, Piotr Godlewski
 * @version 1.0
 * @date 2013-08-01
 */
#ifndef STEINER_TREE_ORACLE_HPP
#define STEINER_TREE_ORACLE_HPP


#include "paal/iterative_rounding/min_cut.hpp"

#include <boost/optional.hpp>

#include <unordered_map>
#include <vector>

namespace paal {
namespace ir {

/**
 * @class steiner_tree_violation_checker
 * @brief Violations checker for the separation oracle
 *      in the steiner tree problem.
 */
class steiner_tree_violation_checker {
    using AuxEdge     = min_cut_finder::Edge;
    using AuxVertex   = min_cut_finder::Vertex;
    using AuxEdgeList = std::vector<AuxEdge>;
    using Violation   = boost::optional<double>;

public:
    using Candidate = AuxVertex;

    steiner_tree_violation_checker() :
        m_current_graph_size(-1) {}

    /**
     * Returns an iterator range of violated constraint candidates.
     */
    template <typename Problem, typename LP>
    auto get_violation_candidates(const Problem & problem, const LP & lp)
            -> decltype(problem.get_terminals()) {

        int graphSize = problem.get_terminals().size();
        if (graphSize != m_current_graph_size) {
            // Graph has changed, construct new oracle
            m_current_graph_size = graphSize;
            m_root = select_root(problem.get_terminals());
            create_auxiliary_digraph(problem, lp);
        } else {
            update_auxiliary_digraph(problem, lp);
        }
        return problem.get_terminals();
    }

    /**
     * Checks if the given constraint candidate is violated an if it is,
     * returns the violation value and violated constraint ID.
     */
    template <typename Problem>
    Violation check_violation(Candidate candidate, const Problem & problem) {
        if (candidate == m_root) {
            return Violation{};
        }

        double violation = check_min_cut(candidate);
        if (problem.get_compare().g(violation, 0)) {
            return violation;
        }
        else {
            return Violation{};
        }
    }

    /**
     * Adds the violated constraint to LP.
     * It contains all the components reachable from a given source,
     * but its sink vertex is not reachable.
     */
    template <typename Problem, typename LP>
    void add_violated_constraint(Candidate violatingTerminal, const Problem & problem, LP & lp) {
        if (std::make_pair(violatingTerminal, m_root) != m_min_cut.get_last_cut()) {
            check_min_cut(violatingTerminal);
        }

        const auto & components = problem.get_components();
        lp::linear_expression expr;
        for (int i = 0; i < components.size(); ++i) {
            auto u = m_artif_vertices[i];
            int ver = components.find_version(i);
            auto v = components.find(i).get_sink(ver);
            if (m_min_cut.is_in_source_set(u) && !m_min_cut.is_in_source_set(v)) {
                expr += problem.find_column_lp(i);
            }
        }
        lp.add_row(std::move(expr) >= 1);
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
    void create_auxiliary_digraph(Problem &problem, const LP & lp) {
        m_min_cut.init(problem.get_terminals().size());
        m_artif_vertices.clear();
        const auto & components = problem.get_components();

        for (int i = 0; i < components.size(); ++i) {
            AuxVertex newV = m_min_cut.add_vertex_to_graph();
            m_artif_vertices[i] = newV;
            int ver = components.find_version(i);
            auto sink = components.find(i).get_sink(ver);
            for (auto w : boost::make_iterator_range(components.find(i).get_elements())) {
                if (w != sink) {
                    double INF = std::numeric_limits<double>::max();
                    m_min_cut.add_edge_to_graph(w, newV, INF);
                } else {
                    lp::col_id x = problem.find_column_lp(i);
                    double colVal = lp.get_col_value(x);
                    m_min_cut.add_edge_to_graph(newV, sink, colVal);
                }
            }
        }
    }

    /**
     * Updates the auxiliary directed graph. Should be performed after each LP iteration.
     */
    template <typename Problem, typename LP>
    void update_auxiliary_digraph(Problem &problem, const LP & lp) {
        const auto & components = problem.get_components();
        for (int i = 0; i < components.size(); ++i) {
            AuxVertex componentV = m_artif_vertices[i];
            int ver = components.find_version(i);
            auto sink = components.find(i).get_sink(ver);
            lp::col_id x = problem.find_column_lp(i);
            double colVal = lp.get_col_value(x);
            m_min_cut.add_edge_to_graph(componentV, sink, colVal);
        }
    }

    /**
     * Select the root terminal. Max-flow will be directed to that vertex during LP oracle execution.
     */
    template <typename Terminals>
    AuxVertex select_root(const Terminals & terminals) {
        //TODO: Maybe it's better to select random vertex rather than first
        AuxVertex ret = *terminals.begin();
        return ret;
    }

    /**
     * Runs a maxflow algorithm between given source and root.
     */
    double check_min_cut(AuxVertex src) {
        double minCut = m_min_cut.find_min_cut(src, m_root);
        return 1 - minCut;
    }

    AuxVertex m_root; // root vertex, sink of all max-flows
    int m_current_graph_size; // size of current graph

    std::unordered_map<int, AuxVertex> m_artif_vertices; // maps componentId to auxGraph vertex

    min_cut_finder m_min_cut;
};

} //ir
} //paal
#endif /* STEINER_TREE_ORACLE_HPP */
