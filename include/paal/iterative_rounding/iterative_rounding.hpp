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
namespace ir {

/**
 * @brief Default Iterative Rounding visitor.
 */
struct trivial_visitor {
    /**
     * @brief Method called after (re)solving the LP.
     */
    template <typename Problem, typename LP>
    void solve_lp_to_extreme_point(Problem & problem, LP & lp) {}

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
 * @tparam LPBase
 */
template <typename Problem, typename IRcomponents, typename Visitor = trivial_visitor, typename LPBase = lp::GLP>
class iterative_rounding  {
    typedef std::unordered_map<lp::col_id, std::pair<double, double>> RoundedCols;

    /**
     * @brief Returns the current value of the LP column.
     */
    double get_val(lp::col_id col) const {
        auto i = m_rounded.find(col);
        if(i == m_rounded.end()) {
            return m_lp_base.get_col_prim(col);
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
        call<Init>(m_problem, m_lp_base);
    }

    /**
     * @brief Finds an extreme point solution to the LP.
     *
     * @return LP solution status
     */
    lp::problem_type solve_lp_to_extreme_point() {
        auto probType = call<SolveLPToExtremePoint>(m_problem, m_lp_base);
        assert(probType != lp::UNDEFINED);
        m_visitor.solve_lp_to_extreme_point(m_problem, m_lp_base);
        return probType;
    }

    /**
     * @brief Finds an extreme point solution to the LP.
     *
     * @return LP solution status
     */
    lp::problem_type resolve_lp_to_extreme_point() {
        auto probType = call<Resolve_lp_to_extreme_point>(m_problem, m_lp_base);
        assert(probType != lp::UNDEFINED);
        m_visitor.solve_lp_to_extreme_point(m_problem, m_lp_base);
        return probType;
    }

    /**
     * @brief Returns the solution cost based on the LP values.
     */
    double get_solution_cost() {
        double solCost(0);
        for(auto col : boost::make_iterator_range(m_lp_base.get_columns())) {
            solCost += m_lp_base.get_col_prim(col) * m_lp_base.get_col_coef(col);
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
        auto cols = m_lp_base.get_columns();

        while (cols.first != cols.second) {
            lp::col_id col = *cols.first;
            auto doRound = call<RoundCondition>(m_problem, m_lp_base, col);
            if (doRound) {
                ++deleted;
                m_rounded.insert(std::make_pair(col,
                    std::make_pair(*doRound, m_lp_base.get_col_coef(col))));
                m_visitor.round_col(m_problem, m_lp_base, col, *doRound);
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
        auto rows = m_lp_base.get_rows();

        while (rows.first != rows.second) {
            lp::row_id row = *rows.first;
            if (call<RelaxCondition>(m_problem, m_lp_base, row)) {
                ++deleted;
                m_visitor.relax_row(m_problem, m_lp_base, row);
                rows.first = m_lp_base.delete_row(rows.first);
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
    LPBase & get_lp() {
        return m_lp_base;
    }

    /**
     * @brief Returns the IR components.
     */
    IRcomponents & get_ir_components() {
        return m_ir_components;
    }

    ///type of functor returning final values for variables
    typedef decltype(std::bind(&iterative_rounding::get_val, std::declval<const iterative_rounding *>(), std::placeholders::_1)) GetSolution;

    /**
     * @brief Sets the solution to the problem using SetSolution component.
     */
    decltype(std::declval<IRcomponents>().template call<SetSolution>(std::declval<Problem &>(), std::declval<GetSolution &>())) set_solution() {
        call<SetSolution>(m_problem, std::bind(&iterative_rounding::get_val, this, std::placeholders::_1));
    }

    /**
     * @brief Rounds the LP using the RoundCondition component.
     */
    void dependent_round() {
         call<RoundCondition>(m_problem, m_lp_base);
    }

    /**
     * @brief Checks if the IR problem has been solved, using the StopCondition component.
     *
     * @return true iff the problem has been solved
     */
    bool stop_condition() {
        return call<StopCondition>(m_problem, m_lp_base);
    }

private:
    template <typename Action, typename... Args>
    auto call(Args&&... args) ->
    decltype(std::declval<IRcomponents>().template call<Action>(std::forward<Args>(args)...)){
        return m_ir_components.template call<Action>(std::forward<Args>(args)...);
    }

    /// Deletes a column from the LP and adjusts the row bounds.
    typename LPBase::ColIter
    delete_column(typename LPBase::ColIter colIter, double value) {
        auto column = m_lp_base.get_rows_in_column(*colIter);
        lp::row_id row;
        double coef;
        for (auto const & c : boost::make_iterator_range(column)) {
            boost::tie(row, coef) = c;
            double currUb = m_lp_base.get_row_ub(row);
            double currLb = m_lp_base.get_row_lb(row);
            lp::bound_type currType = m_lp_base.get_rowbound_type(row);
            double diff = coef * value;
            m_lp_base.set_row_bounds(row, currType, currLb - diff, currUb - diff);
        }
        return m_lp_base.delete_col(colIter);
    };

    LPBase m_lp_base;
    IRcomponents m_ir_components;
    Visitor m_visitor;
    utils::Compare<double> m_compare;
    RoundedCols m_rounded;
    Problem & m_problem;
};


typedef boost::optional<double> IRSolutionCost;
typedef std::pair<lp::problem_type, IRSolutionCost> IRResult;

/**
 * @brief Solves an Iterative Rounding problem.
 *
 * @tparam Problem
 * @tparam IRcomponents
 * @tparam Visitor
 * @tparam LPBase
 * @param problem IR problem
 * @param components IR problem components
 * @param visitor visitor object used for logging progress of the algoithm
 */
template <typename Problem, typename IRcomponents, typename Visitor = trivial_visitor, typename LPBase = lp::GLP>
IRResult solve_iterative_rounding(Problem & problem, IRcomponents components, Visitor visitor = Visitor()) {
    iterative_rounding<Problem, IRcomponents, Visitor, LPBase> ir(problem, std::move(components), std::move(visitor));

    auto probType = ir.solve_lp_to_extreme_point();
    if (probType != lp::OPTIMAL) {
        return IRResult(probType, IRSolutionCost());
    }

    while (!ir.stop_condition()) {
        bool rounded{ir.round()};
        bool relaxed{ir.relax()};
        assert(rounded || relaxed);

        probType = ir.resolve_lp_to_extreme_point();
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
 * @tparam LPBase
 * @param problem IR problem
 * @param components IR problem components
 * @param visitor visitor object used for logging progress of the algoithm
 */
template <typename Problem, typename IRcomponents, typename Visitor = trivial_visitor, typename LPBase = lp::GLP>
IRResult solve_dependent_iterative_rounding(Problem & problem, IRcomponents components, Visitor vis = Visitor()) {
    iterative_rounding<Problem, IRcomponents, Visitor, LPBase> ir(problem, std::move(components), std::move(vis));

    auto probType = ir.solve_lp_to_extreme_point();
    if (probType != lp::OPTIMAL) {
        return IRResult(probType, IRSolutionCost());
    }

    while (!ir.stop_condition()) {
        ir.dependent_round();
        ir.relax();

        probType = ir.resolve_lp_to_extreme_point();
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
