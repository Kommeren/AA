/**
 * @file bounded_degree_mst_oracle.hpp
 * @brief
 * @author Piotr Godlewski
 * @version 1.0
 * @date 2013-06-05
 */
#ifndef BOUNDED_DEGREE_MST_ORACLE_HPP
#define BOUNDED_DEGREE_MST_ORACLE_HPP

#include "paal/iterative_rounding/min_cut.hpp"

#include <boost/optional.hpp>
#include <vector>


namespace paal {
namespace ir {

/**
 * @class bdmst_violation_checker
 * @brief Violations checker for the separation oracle
 *      in the bounded degree minimum spanning tree problem.
 */
class bdmst_violation_checker {
    typedef min_cut_finder::Edge AuxEdge;
    typedef min_cut_finder::Vertex AuxVertex;
    typedef std::vector<AuxEdge> AuxEdgeList;
    typedef boost::optional<double> Violation;

public:
    typedef std::pair<AuxVertex, AuxVertex> Candidate;
    typedef std::vector<Candidate> CandidateList;

    /**
     * Returns an iterator range of violated constraint candidates.
     */
    template <typename Problem, typename LP>
    const CandidateList & get_violation_candidates(const Problem & problem, const LP & lp) {
        fill_auxiliary_digraph(problem, lp);
        initialize_candidates(problem);
        return m_candidate_list;
    }

    /**
     * Checks if the given constraint candidate is violated an if it is,
     * returns the violation value and violated constraint ID.
     */
    template <typename Problem>
    Violation check_violation(Candidate candidate, const Problem & problem) {
        double violation = check_min_cut(candidate.first, candidate.second);
        if (problem.get_compare().g(violation, 0)) {
            return Violation(violation);
        }
        else {
            return Violation();
        }
    }

    /**
     * Adds a violated constraint to the LP.
     */
    template <typename Problem, typename LP>
    void add_violated_constraint(Candidate violatingPair, const Problem & problem, LP & lp) {
        if (violatingPair != m_min_cut.get_last_cut()) {
            check_min_cut(violatingPair.first, violatingPair.second);
        }

        const auto & g = problem.get_graph();
        lp.add_row(lp::UP, 0, m_min_cut.source_set_size() - 2);

        for (auto const & e : problem.get_edge_map().right) {
            auto u = source(e.second, g);
            auto v = target(e.second, g);
            if (m_min_cut.is_in_source_set(u) && m_min_cut.is_in_source_set(v)) {
                lp.add_new_row_coef(e.first);
            }
        }

        lp.load_new_row();
    }

private:

    /**
     * Creates the auxiliary directed graph used for feasibility testing.
     */
    template <typename Problem, typename LP>
    void fill_auxiliary_digraph(const Problem & problem, const LP & lp) {
        const auto & g = problem.get_graph();
        m_vertices_num = num_vertices(g);
        m_min_cut.init(m_vertices_num);
        m_src_to_v.resize(m_vertices_num);
        m_v_to_trg.resize(m_vertices_num);

        for (auto const & e : problem.get_edge_map().right) {
            lp::col_id colIdx = e.first;
            double colVal = lp.get_col_prim(colIdx) / 2;

            if (!problem.get_compare().e(colVal, 0)) {
                auto u = source(e.second, g);
                auto v = target(e.second, g);
                m_min_cut.add_edge_to_graph(u, v, colVal, colVal);
            }
        }

        m_src = m_min_cut.add_vertex_to_graph();
        m_trg = m_min_cut.add_vertex_to_graph();

        for (auto v : boost::make_iterator_range(vertices(g))) {
            m_src_to_v[v] = m_min_cut.add_edge_to_graph(m_src, v, degree_of(problem, v, lp) / 2).first;
            m_v_to_trg[v] = m_min_cut.add_edge_to_graph(v, m_trg, 1).first;
        }
    }

    /**
     * Initializes the list of cut candidates.
     */
    template <typename Problem>
    void initialize_candidates(const Problem & problem) {
        const auto & g = problem.get_graph();
        auto src = *(std::next(vertices(g).first, rand() % m_vertices_num));
        m_candidate_list.clear();
        for (auto v : boost::make_iterator_range(vertices(g))) {
            if (v != src) {
                m_candidate_list.push_back(std::make_pair(src, v));
                m_candidate_list.push_back(std::make_pair(v, src));
            }
        }
    }

    /**
     * Calculates the sum of the variables for edges incident with a given vertex.
     */
    template <typename Problem, typename LP, typename Vertex>
    double degree_of(const Problem & problem, const Vertex & v, const LP & lp) {
        double res = 0;
        auto adjEdges = out_edges(v, problem.get_graph());

        for (auto e : boost::make_iterator_range(adjEdges)) {
            auto colId = problem.edge_to_col(e);
            if (colId) {
                res += lp.get_col_prim(*colId);
            }
        }
        return res;
    }

    /**
     * Finds the most violated set of vertices containing \c src and not containing \c trg.
     * @param src vertex to be contained in the violating set
     * @param trg vertex not to be contained in the violating set
     * @return violation of the found set
     */
    double check_min_cut(AuxVertex src, AuxVertex trg) {
        double origCap = m_min_cut.get_capacity(m_src_to_v[src]);

        m_min_cut.set_capacity(m_src_to_v[src], m_vertices_num);
        // capacity of srcToV[trg] does not change
        m_min_cut.set_capacity(m_v_to_trg[src], 0);
        m_min_cut.set_capacity(m_v_to_trg[trg], m_vertices_num);

        double minCut = m_min_cut.find_min_cut(m_src, m_trg);
        double violation = m_vertices_num - 1 - minCut;

        // reset the original values for the capacities
        m_min_cut.set_capacity(m_src_to_v[src], origCap);
        // capacity of srcToV[trg] does not change
        m_min_cut.set_capacity(m_v_to_trg[src], 1);
        m_min_cut.set_capacity(m_v_to_trg[trg], 1);

        return violation;
    }

    int m_vertices_num;

    AuxVertex   m_src;
    AuxVertex   m_trg;

    AuxEdgeList  m_src_to_v;
    AuxEdgeList  m_v_to_trg;

    CandidateList m_candidate_list;

    min_cut_finder m_min_cut;
};


} //ir
} //paal
#endif /* BOUNDED_DEGREE_MST_ORACLE_HPP */
