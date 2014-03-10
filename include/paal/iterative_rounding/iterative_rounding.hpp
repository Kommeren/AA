/**
 * @file iterative_rounding.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-05-06
 */
#ifndef ITERATIVE_ROUNDING_HPP
#define ITERATIVE_ROUNDING_HPP


#include "paal/utils/type_functions.hpp"
#include "paal/utils/floating.hpp"
#include "paal/lp/glp.hpp"
#include "paal/iterative_rounding/ir_components.hpp"

#include <boost/range/irange.hpp>
#include <boost/optional.hpp>

#include <cstdlib>
#include <unordered_map>


namespace paal {
/// Iterative Rounding namespace.
namespace ir {

/**
 * @brief Default Iterative Rounding visitor.
 */
struct trivial_visitor {
    /**
     * @brief Method called after (re)solving the LP.
     */
    template <typename Problem, typename LP>
    void solve_lp(Problem & problem, LP & lp) {}

    /**
     * @brief Method called after rounding a column of the LP.
     */
    template <typename Problem, typename LP>
    void round_col(Problem & problem, LP & lp, lp::col_id col, double val) {}

    /**
     * @brief Method called after relaxing a row of the LP.
     */
    template <typename Problem, typename LP>
    void relax_row(Problem & problem, LP & lp, lp::row_id row) {}
};

/**
 * @brief This class solves an iterative rounding problem.
 *
 * @tparam Problem
 * @tparam IRcomponents
 * @tparam Visitor
 * @tparam LP
 */
template <typename Problem, typename IRcomponents, typename Visitor = trivial_visitor, typename LP = lp::GLP>
class iterative_rounding  {
    typedef std::unordered_map<lp::col_id, std::pair<double, double>> RoundedCols;

    /**
     * @brief Returns the current value of the LP column.
     */
    double get_val(lp::col_id col) const {
        auto i = m_rounded.find(col);
        if(i == m_rounded.end()) {
            return m_lp.get_col_prim(col);
        } else {
            return i->second.first;
        }
    }

public:
    /**
     * @brief Constructor.
     */
    iterative_rounding(Problem & problem, IRcomponents e, Visitor vis = Visitor())
        : m_ir_components(std::move(e)), m_visitor(std::move(vis)), m_problem(problem) {
        call<Init>(m_problem, m_lp);
    }

    /**
     * @brief Finds solution to the LP.
     *
     * @return LP solution status
     */
    lp::problem_type solve_lp() {
        auto probType = call<SolveLP>(m_problem, m_lp);
        assert(probType != lp::UNDEFINED);
        m_visitor.solve_lp(m_problem, m_lp);
        return probType;
    }

    /**
     * @brief Finds solution to the LP.
     *
     * @return LP solution status
     */
    lp::problem_type resolve_lp() {
        auto probType = call<ResolveLP>(m_problem, m_lp);
        assert(probType != lp::UNDEFINED);
        m_visitor.solve_lp(m_problem, m_lp);
        return probType;
    }

    /**
     * @brief Returns the solution cost based on the LP values.
     */
    double get_solution_cost() {
        double solCost(0);
        for(auto col : boost::make_iterator_range(m_lp.get_columns())) {
            solCost += m_lp.get_col_prim(col) * m_lp.get_col_coef(col);
        }
        for (auto roundedCol : m_rounded) {
            solCost += roundedCol.second.first * roundedCol.second.second;
        }
        return solCost;
    }

    /**
     * @brief Rounds the LP columns (independently) using the RoundCondition component.
     *
     * @return true iff at least one column was rounded
     */
    bool round() {
        int deleted(0);
        auto cols = m_lp.get_columns();

        while (cols.first != cols.second) {
            lp::col_id col = *cols.first;
            auto doRound = call<RoundCondition>(m_problem, m_lp, col);
            if (doRound) {
                ++deleted;
                m_rounded.insert(std::make_pair(col,
                    std::make_pair(*doRound, m_lp.get_col_coef(col))));
                m_visitor.round_col(m_problem, m_lp, col, *doRound);
                cols.first = delete_column(cols.first, *doRound);
            }
            else {
                ++cols.first;
            }
        }

        return deleted > 0;
    }

    /**
     * @brief Relaxes the LP rows using the RelaxCondition component.
     *
     * @return true iff at least one row was relaxed
     */
    bool relax() {
        int deleted(0);
        auto rows = m_lp.get_rows();

        while (rows.first != rows.second) {
            lp::row_id row = *rows.first;
            if (call<RelaxCondition>(m_problem, m_lp, row)) {
                ++deleted;
                m_visitor.relax_row(m_problem, m_lp, row);
                rows.first = m_lp.delete_row(rows.first);
                if (call<RelaxationsLimit>(deleted)) {
                    break;
                }
            }
            else {
                ++rows.first;
            }
        }

        return deleted > 0;
    }

    /**
     * @brief Returns the LP object used to solve the IR.
     */
    LP & get_lp() {
        return m_lp;
    }

    /**
     * @brief Returns the IR components.
     */
    IRcomponents & get_ir_components() {
        return m_ir_components;
    }

    /**
     * @brief Sets the solution to the problem using SetSolution component.
     */
    void set_solution() {
        call<SetSolution>(m_problem, std::bind(&iterative_rounding::get_val, this, std::placeholders::_1));
    }

    /**
     * @brief Rounds the LP using the RoundCondition component.
     */
    void dependent_round() {
         call<RoundCondition>(m_problem, m_lp);
    }

    /**
     * @brief Checks if the IR problem has been solved, using the StopCondition component.
     *
     * @return true iff the problem has been solved
     */
    bool stop_condition() {
        return call<StopCondition>(m_problem, m_lp);
    }

private:
    template <typename Action, typename... Args>
    auto call(Args&&... args) ->
    decltype(std::declval<IRcomponents>().template call<Action>(std::forward<Args>(args)...)){
        return m_ir_components.template call<Action>(std::forward<Args>(args)...);
    }

    /// Deletes a column from the LP and adjusts the row bounds.
    typename LP::ColIter
    delete_column(typename LP::ColIter colIter, double value) {
        auto column = m_lp.get_rows_in_column(*colIter);
        lp::row_id row;
        double coef;
        for (auto const & c : boost::make_iterator_range(column)) {
            boost::tie(row, coef) = c;
            double currUb = m_lp.get_row_ub(row);
            double currLb = m_lp.get_row_lb(row);
            lp::bound_type currType = m_lp.get_rowbound_type(row);
            double diff = coef * value;
            m_lp.set_row_bounds(row, currType, currLb - diff, currUb - diff);
        }
        return m_lp.delete_col(colIter);
    };

    LP m_lp;
    IRcomponents m_ir_components;
    Visitor m_visitor;
    utils::Compare<double> m_compare;
    RoundedCols m_rounded;
    Problem & m_problem;
};


/// Iterative Rounding solution cost type. Solution cost only makes sense if the LP has been solved to optimal value.
typedef boost::optional<double> IRSolutionCost;
/// Iterative Rounding result type: Pair consisting of LP problem type and IR solution cost.
typedef std::pair<lp::problem_type, IRSolutionCost> IRResult;

/**
 * @brief Solves an Iterative Rounding problem.
 *
 * @tparam Problem
 * @tparam IRcomponents
 * @tparam Visitor
 * @tparam LP
 * @param problem IR problem
 * @param components IR problem components
 * @param visitor visitor object used for logging progress of the algoithm
 */
template <typename Problem, typename IRcomponents, typename Visitor = trivial_visitor, typename LP = lp::GLP>
IRResult solve_iterative_rounding(Problem & problem, IRcomponents components, Visitor visitor = Visitor()) {
    iterative_rounding<Problem, IRcomponents, Visitor, LP> ir(problem, std::move(components), std::move(visitor));

    auto probType = ir.solve_lp();
    if (probType != lp::OPTIMAL) {
        return IRResult(probType, IRSolutionCost());
    }

    while (!ir.stop_condition()) {
        bool rounded{ir.round()};
        bool relaxed{ir.relax()};
        assert(rounded || relaxed);

        probType = ir.resolve_lp();
        if (probType != lp::OPTIMAL) {
            return IRResult(probType, IRSolutionCost());
        }
    }
    ir.set_solution();
    return IRResult(lp::OPTIMAL, IRSolutionCost(ir.get_solution_cost()));
}

/**
 * @brief Solves an Iterative Rounding problem with dependent rounding.
 *
 * @tparam Problem
 * @tparam IRcomponents
 * @tparam Visitor
 * @tparam LP
 * @param problem IR problem
 * @param components IR problem components
 * @param visitor visitor object used for logging progress of the algoithm
 */
template <typename Problem, typename IRcomponents, typename Visitor = trivial_visitor, typename LP = lp::GLP>
IRResult solve_dependent_iterative_rounding(Problem & problem, IRcomponents components, Visitor visitor = Visitor()) {
    iterative_rounding<Problem, IRcomponents, Visitor, LP> ir(problem, std::move(components), std::move(visitor));

    auto probType = ir.solve_lp();
    if (probType != lp::OPTIMAL) {
        return IRResult(probType, IRSolutionCost());
    }

    while (!ir.stop_condition()) {
        ir.dependent_round();
        ir.relax();

        probType = ir.resolve_lp();
        if (probType != lp::OPTIMAL) {
            return IRResult(probType, IRSolutionCost());
        }
    }
    ir.set_solution();
    return IRResult(lp::OPTIMAL, IRSolutionCost(ir.get_solution_cost()));
}

} //ir
} //paal


#endif //ITERATIVE_ROUNDING_HPP
