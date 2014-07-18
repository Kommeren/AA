/**
 * @file steiner_tree.hpp
 * @brief
 * @author Maciej Andrejczuk, Piotr Godlewski, Piotr Wygocki
 * @version 1.0
 * @date 2013-08-01
 */
#ifndef STEINER_TREE_HPP
#define STEINER_TREE_HPP

#define BOOST_RESULT_OF_USE_DECLTYPE


#include "paal/data_structures/metric/basic_metrics.hpp"
#include "paal/iterative_rounding/ir_components.hpp"
#include "paal/iterative_rounding/iterative_rounding.hpp"
#include "paal/iterative_rounding/steiner_tree/steiner_components.hpp"
#include "paal/iterative_rounding/steiner_tree/steiner_strategy.hpp"
#include "paal/iterative_rounding/steiner_tree/steiner_tree_oracle.hpp"
#include "paal/iterative_rounding/steiner_tree/steiner_utils.hpp"
#include "paal/lp/lp_row_generation.hpp"
#include "paal/lp/separation_oracles.hpp"
#include "paal/utils/floating.hpp"

#include <boost/range/join.hpp>
#include <boost/range/irange.hpp>

namespace paal {
namespace ir {

namespace {
struct steiner_tree_compare_traits {
    static const double EPSILON;
};

const double steiner_tree_compare_traits::EPSILON = 1e-10;
}

template <template <typename> class OracleStrategy =
              lp::random_violated_separation_oracle>
using steiner_tree_oracle = OracleStrategy<steiner_tree_violation_checker>;

/**
 * @class steiner_tree
 * @brief The class for solving the Steiner Tree problem using Iterative Rounding.
 *
 * @tparam OrigMetric
 * @tparam Terminals
 * @tparam Result
 * @tparam Strategy
 * @tparam Oracle separation oracle
 */
template<typename OrigMetric, typename Terminals, typename Result,
    typename Strategy = all_generator,
    typename Oracle = steiner_tree_oracle<>>
class steiner_tree {
public:
    using MT = data_structures::metric_traits<OrigMetric>;
    using Vertex = typename MT::VertexType;
    using Dist = typename MT::DistanceType;
    using Edge = typename std::pair<Vertex, Vertex>;
    using Compare = utils::compare<double>;
    using VertexIndex = data_structures::bimap<Vertex>;
    using MetricIdx = data_structures::array_metric<Dist>;
    using Metric = data_structures::metric_on_idx<
                        data_structures::array_metric<Dist> &,
                        const VertexIndex &,
                        data_structures::read_values_tag>;

private:
    Terminals m_terminals;        // terminals in current state
    Terminals m_steiner_vertices; // vertices that are not terminals
    VertexIndex m_vertex_index; //mapping vertices to numbers for 0 to n.
    MetricIdx m_cost_map_idx;            // metric in current state (operates on indexes)
    Metric m_cost_map;            // metric in current state
    steiner_components<Vertex, Dist> m_components; // components in current
                                                   // state
    Strategy m_strategy;      // strategy to generate the components
    Result m_result_iterator; // list of selected Steiner Vertices
    Compare m_compare;        // comparison method

    std::unordered_map<int, lp::col_id> m_elements_map; // maps componentId ->
                                                        // col_id in LP

    Oracle m_oracle;

public:
    /**
     * Constructor.
     */
    steiner_tree(const OrigMetric& metric, const Terminals& terminals,
            const Terminals& steiner_vertices, Result result,
            const Strategy& strategy = Strategy{}, Oracle oracle = Oracle{}) :
        m_terminals(terminals), m_steiner_vertices(steiner_vertices),
        m_vertex_index(boost::range::join(m_terminals, m_steiner_vertices)),
        m_cost_map_idx(metric, boost::range::join(m_terminals, m_steiner_vertices)),
        m_cost_map(m_cost_map_idx, m_vertex_index),
        m_strategy(strategy), m_result_iterator(result),
        m_compare(steiner_tree_compare_traits::EPSILON), m_oracle(oracle) {
    }

    /**
     * Returns the separation oracle.
     */
    Oracle &get_oracle() { return m_oracle; }

    /**
     * Generates all the components using specified strategy.
     */
    void gen_components() {
        m_strategy.gen_components(m_cost_map, m_terminals, m_steiner_vertices,
                                  m_components);
    }

    /**
     * Gets reference to all the components.
     */
    const steiner_components<Vertex, Dist> &get_components() const {
        return m_components;
    }

    /**
     * Gets reference to all the terminals.
     */
    const Terminals &get_terminals() const { return m_terminals; }

    ///return idx of the vertex
    auto get_idx(Vertex v)  const -> decltype(m_vertex_index.get_idx(v)) {
        return m_vertex_index.get_idx(v);
    }

    /**
     * Adds map entry from component id to LP lp::col_id.
     */
    void add_column_lp(int id, lp::col_id col) {
        bool b = m_elements_map.insert(std::make_pair(id, col)).second;
        assert(b);
    }

    /**
     * Finds LP lp::col_id based on component id.
     */
    lp::col_id find_column_lp(int id) const { return m_elements_map.at(id); }

    /**
     * Adds elements to solution.
     */
    void add_to_solution(const std::vector<Vertex>& steiner_elements) {
        std::copy(steiner_elements.begin(), steiner_elements.end(), m_result_iterator);
    }

    /**
     * Recalculates distances after two vertices were merged.
     */
    void merge_vertices(Vertex u_vertex, Vertex w_vertex) {
        auto all_elements = boost::irange(0, int(boost::distance(m_terminals)
                                           + boost::distance(m_steiner_vertices)));
        auto u = get_idx(u_vertex);
        auto w = get_idx(w_vertex);
        for (auto i: all_elements) {
            for (auto j: all_elements) {
                Dist x = m_cost_map_idx(i, u) + m_cost_map_idx(w, j);
                m_cost_map_idx(i, j) = std::min(m_cost_map_idx(i, j), x);
            }
        }
    }

    /**
     * Merges a component into its sink.
     */
    void update_graph(const steiner_component<Vertex, Dist>& selected) {
        const std::vector<Vertex>& v = selected.get_terminals();
        auto all_elements_except_first = boost::make_iterator_range(++v.begin(), v.end());
        for (auto e : all_elements_except_first) {
            merge_vertices(v[0], e);
            auto ii = std::find(m_terminals.begin(), m_terminals.end(), e);
            assert(ii != m_terminals.end());
            m_terminals.erase(ii);
        }
        // Clean components, they will be generated once again
        m_components.clear();
        m_elements_map.clear();
    }

    /**
     * Gets comparison method.
     */
    utils::compare<double> get_compare() const {
        return m_compare;
    }

};


/**
 * Initialization of the IR Steiner Tree algorithm.
 */
class steiner_tree_init {
  public:
    /**
     * Initializes LP.
     */
    template <typename Problem, typename LP>
    void operator()(Problem &problem, LP &lp) {
        lp.clear();
        lp.set_lp_name("steiner tree");
        problem.gen_components();
        lp.set_optimization_type(lp::MINIMIZE);
        add_variables(problem, lp);
    }

  private:
    /**
     * Adds all the components as columns of LP.
     */
    template <typename Problem, typename LP>
    void add_variables(Problem &problem, LP &lp) {
        for (int i = 0; i < problem.get_components().size(); ++i) {
            lp::col_id col = lp.add_column(
                problem.get_components().find(i).get_cost(), 0, 1);
            problem.add_column_lp(i, col);
        }
    }
};

/**
 * Round Condition: step of iterative-randomized rounding algorithm.
 */
class steiner_tree_round_condition {
  public:
    steiner_tree_round_condition() {}

    /**
     * Selects one component according to probability, adds it to solution and
     * merges selected vertices.
     */
    template <typename Problem, typename LP>
    void operator()(Problem &problem, LP &lp) {
        std::vector<double> weights;
        weights.reserve(problem.get_components().size());
        for (int i = 0; i < problem.get_components().size(); ++i) {
            lp::col_id cId = problem.find_column_lp(i);
            weights.push_back(lp.get_col_value(cId));
        }
        int selected =
            paal::utils::random_select(weights.begin(), weights.end()) -
            weights.begin();
        const auto &comp = problem.get_components().find(selected);
        problem.add_to_solution(comp.get_steiner_elements());
        problem.update_graph(comp);
        steiner_tree_init()(problem, lp);
    }
};

/**
 * Stop Condition: step of iterative-randomized rounding algorithm.
 */
class steiner_tree_stop_condition {
public:
    /**
     * Checks if the IR algorithm should terminate.
     */
    template<typename Problem, typename LP>
    bool operator()(Problem& problem, LP &) {
        return problem.get_terminals().size() < 2;
    }
};

/**
 * Makes steiner_tree object. Just to avoid providing type names in template.
 */
template<typename Oracle = steiner_tree_oracle<>,
        typename OrigMetric, typename Terminals, typename Result, typename Strategy>
steiner_tree<OrigMetric, Terminals, Result, Strategy, Oracle> make_steiner_tree(
        const OrigMetric& metric, const Terminals& terminals,
        const Terminals& steiner_vertices, Result result, const Strategy& strategy,
        Oracle oracle = Oracle()) {
    return steiner_tree<OrigMetric, Terminals, Result, Strategy, Oracle>(metric,
            terminals, steiner_vertices, result, strategy, oracle);
}

template <typename Init = steiner_tree_init,
          typename RoundCondition = steiner_tree_round_condition,
          typename RelaxCondition = utils::always_false,
          typename SetSolution = utils::skip_functor,
          typename SolveLPToExtremePoint = lp::row_generation_solve_lp,
          typename ResolveLPToExtremePoint = lp::row_generation_solve_lp,
          typename StopCondition = steiner_tree_stop_condition>
using steiner_tree_ir_components =
    IRcomponents<Init, RoundCondition, RelaxCondition, SetSolution,
                 SolveLPToExtremePoint, ResolveLPToExtremePoint, StopCondition>;

/**
 * @brief Solves the Steiner Tree problem using Iterative Rounding.
 *
 * @tparam Oracle
 * @tparam Strategy
 * @tparam OrigMetric
 * @tparam Terminals
 * @tparam Result
 * @tparam IRComponents
 * @tparam Visitor
 * @param metric
 * @param terminals
 * @param steiner_vertices
 * @param result
 * @param strategy
 * @param comps
 * @param oracle
 * @param visitor
 */
template <typename Oracle = steiner_tree_oracle<>, typename Strategy = all_generator,
    typename OrigMetric, typename Terminals, typename Result,
    typename IRcomponents = steiner_tree_ir_components<>, typename Visitor = trivial_visitor>
void steiner_tree_iterative_rounding(const OrigMetric& metric, const Terminals& terminals, const Terminals& steiner_vertices,
        Result result, Strategy strategy, IRcomponents comps = IRcomponents{},
        Oracle oracle = Oracle{}, Visitor visitor = Visitor{}) {

    auto steiner = paal::ir::make_steiner_tree(metric, terminals, steiner_vertices, result, strategy, oracle);
    paal::ir::solve_dependent_iterative_rounding(steiner, std::move(comps), std::move(visitor));
}

} //! ir
} //! paal
#endif /* STEINER_TREE_HPP */
