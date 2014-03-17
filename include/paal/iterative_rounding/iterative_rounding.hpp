/**
 * @file iterative_rounding.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-05-06
 */
#ifndef ITERATIVE_ROUNDING_HPP
#define ITERATIVE_ROUNDING_HPP

#include <cstdlib>
#include <unordered_map>

#include <boost/range/irange.hpp>
#include <boost/optional.hpp>

#include "paal/utils/type_functions.hpp"
#include "paal/utils/floating.hpp"
#include "paal/lp/glp.hpp"
#include "paal/iterative_rounding/ir_components.hpp"


namespace paal {
namespace ir {

/**
 * @brief Default Iterative Rounding visitor.
 */
struct TrivialVisitor {
    /**
     * @brief Method called after (re)solving the LP.
     */
    template <typename Problem, typename LP>
    void solveLPToExtremePoint(Problem & problem, LP & lp) {}

    /**
     * @brief Method called after rounding a column of the LP.
     */
    template <typename Problem, typename LP>
    void roundCol(Problem & problem, LP & lp, lp::ColId col, double val) {}

    /**
     * @brief Method called after relaxing a row of the LP.
     */
    template <typename Problem, typename LP>
    void relaxRow(Problem & problem, LP & lp, lp::RowId row) {}
};

/**
 * @brief This class solves an iterative rounding problem.
 *
 * @tparam Problem
 * @tparam IRComponents
 * @tparam Visitor
 * @tparam LPBase
 */
template <typename Problem, typename IRComponents, typename Visitor = TrivialVisitor, typename LPBase = lp::GLP>
class IterativeRounding  {
    typedef std::unordered_map<lp::ColId, std::pair<double, double>> RoundedCols;

    /**
     * @brief Returns the current value of the LP column.
     */
    double getVal(lp::ColId col) const {
        auto i = m_rounded.find(col);
        if(i == m_rounded.end()) {
            return m_lpBase.getColPrim(col);
        } else {
            return i->second.first;
        }
    }

public:
    /**
     * @brief Constructor.
     */
    IterativeRounding(Problem & problem, IRComponents e, Visitor vis = Visitor())
        : m_irComponents(std::move(e)), m_visitor(std::move(vis)), m_problem(problem) {
        call<Init>(m_problem, m_lpBase);
    }

    /**
     * @brief Finds an extreme point solution to the LP.
     *
     * @return LP solution status
     */
    lp::ProblemType solveLPToExtremePoint() {
        auto probType = call<SolveLPToExtremePoint>(m_problem, m_lpBase);
        assert(probType != lp::UNDEFINED);
        m_visitor.solveLPToExtremePoint(m_problem, m_lpBase);
        return probType;
    }

    /**
     * @brief Finds an extreme point solution to the LP.
     *
     * @return LP solution status
     */
    lp::ProblemType resolveLPToExtremePoint() {
        auto probType = call<ResolveLPToExtremePoint>(m_problem, m_lpBase);
        assert(probType != lp::UNDEFINED);
        m_visitor.solveLPToExtremePoint(m_problem, m_lpBase);
        return probType;
    }

    /**
     * @brief Returns the solution cost based on the LP values.
     */
    double getSolutionCost() {
        double solCost(0);
        for(auto col : boost::make_iterator_range(m_lpBase.getColumns())) {
            solCost += m_lpBase.getColPrim(col) * m_lpBase.getColCoef(col);
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
        auto cols = m_lpBase.getColumns();

        while (cols.first != cols.second) {
            lp::ColId col = *cols.first;
            auto doRound = call<RoundCondition>(m_problem, m_lpBase, col);
            if (doRound) {
                ++deleted;
                m_rounded.insert(std::make_pair(col,
                    std::make_pair(*doRound, m_lpBase.getColCoef(col))));
                m_visitor.roundCol(m_problem, m_lpBase, col, *doRound);
                cols.first = deleteColumn(cols.first, *doRound);
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
        auto rows = m_lpBase.getRows();

        while (rows.first != rows.second) {
            lp::RowId row = *rows.first;
            if (call<RelaxCondition>(m_problem, m_lpBase, row)) {
                ++deleted;
                m_visitor.relaxRow(m_problem, m_lpBase, row);
                rows.first = m_lpBase.deleteRow(rows.first);
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
    LPBase & getLP() {
        return m_lpBase;
    }

    /**
     * @brief Returns the IR components.
     */
    IRComponents & getIRComponents() {
        return m_irComponents;
    }

    ///type of functor returning final values for variables
    typedef decltype(std::bind(&IterativeRounding::getVal, std::declval<const IterativeRounding *>(), std::placeholders::_1)) GetSolution;

    /**
     * @brief Sets the solution to the problem using SetSolution component.
     */
    decltype(std::declval<IRComponents>().template call<SetSolution>(std::declval<Problem &>(), std::declval<GetSolution &>())) setSolution() {
        call<SetSolution>(m_problem, std::bind(&IterativeRounding::getVal, this, std::placeholders::_1));
    }

    /**
     * @brief Rounds the LP using the RoundCondition component.
     */
    void dependentRound() {
         call<RoundCondition>(m_problem, m_lpBase);
    }

    /**
     * @brief Checks if the IR problem has been solved, using the StopCondition component.
     *
     * @return true iff the problem has been solved
     */
    bool stopCondition() {
        return call<StopCondition>(m_problem, m_lpBase);
    }

private:
    template <typename Action, typename... Args>
    auto call(Args&&... args) ->
    decltype(std::declval<IRComponents>().template call<Action>(std::forward<Args>(args)...)){
        return m_irComponents.template call<Action>(std::forward<Args>(args)...);
    }

    /// Deletes a column from the LP and adjusts the row bounds.
    typename LPBase::ColIter
    deleteColumn(typename LPBase::ColIter colIter, double value) {
        auto column = m_lpBase.getRowsInColumn(*colIter);
        lp::RowId row;
        double coef;
        for (auto const & c : boost::make_iterator_range(column)) {
            boost::tie(row, coef) = c;
            double currUb = m_lpBase.getRowUb(row);
            double currLb = m_lpBase.getRowLb(row);
            lp::BoundType currType = m_lpBase.getRowBoundType(row);
            double diff = coef * value;
            m_lpBase.setRowBounds(row, currType, currLb - diff, currUb - diff);
        }
        return m_lpBase.deleteCol(colIter);
    };

    LPBase m_lpBase;
    IRComponents m_irComponents;
    Visitor m_visitor;
    utils::Compare<double> m_compare;
    RoundedCols m_rounded;
    Problem & m_problem;
};


typedef boost::optional<double> IRSolutionCost;
typedef std::pair<lp::ProblemType, IRSolutionCost> IRResult;

/**
 * @brief Solves an Iterative Rounding problem.
 *
 * @tparam Problem
 * @tparam IRComponents
 * @tparam Visitor
 * @tparam LPBase
 * @param problem IR problem
 * @param components IR problem components
 * @param visitor visitor object used for logging progress of the algoithm
 */
template <typename Problem, typename IRComponents, typename Visitor = TrivialVisitor, typename LPBase = lp::GLP>
IRResult solve_iterative_rounding(Problem & problem, IRComponents components, Visitor visitor = Visitor()) {
    IterativeRounding<Problem, IRComponents, Visitor, LPBase> ir(problem, std::move(components), std::move(visitor));

    auto probType = ir.solveLPToExtremePoint();
    if (probType != lp::OPTIMAL) {
        return IRResult(probType, IRSolutionCost());
    }

    while (!ir.stopCondition()) {
        bool rounded{ir.round()};
        bool relaxed{ir.relax()};
        assert(rounded || relaxed);

        probType = ir.resolveLPToExtremePoint();
        if (probType != lp::OPTIMAL) {
            return IRResult(probType, IRSolutionCost());
        }
    }
    ir.setSolution();
    return IRResult(lp::OPTIMAL, IRSolutionCost(ir.getSolutionCost()));
}

/**
 * @brief Solves an Iterative Rounding problem with dependent rounding.
 *
 * @tparam Problem
 * @tparam IRComponents
 * @tparam Visitor
 * @tparam LPBase
 * @param problem IR problem
 * @param components IR problem components
 * @param visitor visitor object used for logging progress of the algoithm
 */
template <typename Problem, typename IRComponents, typename Visitor = TrivialVisitor, typename LPBase = lp::GLP>
IRResult solve_dependent_iterative_rounding(Problem & problem, IRComponents components, Visitor vis = Visitor()) {
    IterativeRounding<Problem, IRComponents, Visitor, LPBase> ir(problem, std::move(components), std::move(vis));

    auto probType = ir.solveLPToExtremePoint();
    if (probType != lp::OPTIMAL) {
        return IRResult(probType, IRSolutionCost());
    }

    while (!ir.stopCondition()) {
        ir.dependentRound();
        ir.relax();

        probType = ir.resolveLPToExtremePoint();
        if (probType != lp::OPTIMAL) {
            return IRResult(probType, IRSolutionCost());
        }
    }
    ir.setSolution();
    return IRResult(lp::OPTIMAL, IRSolutionCost(ir.getSolutionCost()));
}

} //ir
} //paal


#endif //ITERATIVE_ROUNDING_HPP
