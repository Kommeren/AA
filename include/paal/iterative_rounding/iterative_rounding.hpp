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

#include "paal/utils/type_functions.hpp"
#include "paal/utils/double_rounding.hpp"
#include "paal/lp/glp.hpp"
#include "paal/iterative_rounding/ir_components.hpp"


namespace paal {
namespace ir {

struct TrivialVisitor {
    template <typename Problem, typename LP>
    void solveLPToExtremePoint(Problem & problem, LP & lp) {}

    template <typename Problem, typename LP>
    void roundCol(Problem & problem, LP & lp, ColId col, double val) {}
    
    template <typename Problem, typename LP>
    void relaxRow(Problem & problem, LP & lp, RowId row) {}
};

template <typename Problem, typename IRComponents, typename Visitor = TrivialVisitor, typename LPBase = GLP>
class IterativeRounding  {
    double getVal(ColId col) const {
        auto i = m_rounded.find(col.get());
        if(i == m_rounded.end()) {
            return m_lpBase.getColPrim(col);
        } else {
            return i->second;
        }
    }

public:
    IterativeRounding(Problem & problem, IRComponents e, Visitor vis = Visitor()) 
        : m_irComponents(std::move(e)), m_visitor(vis), m_problem(problem) {
        call<Init>(m_problem, m_lpBase);
    }
    
    ProblemType solveLPToExtremePoint() {
        auto probType = call<SolveLPToExtremePoint>(m_problem, m_lpBase);
        assert(probType != UNDEFINED);
        m_visitor.solveLPToExtremePoint(m_problem, m_lpBase);
        return probType;
    }

    bool integerSolution() {
        for(ColId col :boost::make_iterator_range(m_lpBase.getColumns())) {
            double colVal = m_lpBase.getColPrim(col);
            if (!m_compare.e(colVal, std::round(colVal))) {
                return false;
            }
        }
        
        return true;
    }
    
    bool round() {
        int deleted(0);
        bool repeat = true;
        //TODO optimize
        while(repeat) {
            repeat = false;
            for(auto cols = m_lpBase.getColumns(); cols.first != cols.second; ++cols.first) {
                ColId col = *cols.first;
                if (m_lpBase.getColBoundType(col) != FX) {
                    auto doRound = call<RoundCondition>(m_problem, m_lpBase, col);
                    if(doRound) {
                        m_rounded.insert(std::make_pair(col.get(), *doRound));
                        ++deleted;
                        m_visitor.roundCol(m_problem, m_lpBase, col, *doRound);
                        call<DeleteColStrategy>(m_lpBase, col, *doRound);
                        repeat = true;
                        break;
                    }
                }
            }
        }
        
        return deleted > 0;
    }
    
    bool relax() {
        int deleted(0);
        bool repeat = true;
        //TODO optimize
        while(repeat) {
            repeat = false;

            for(auto rows = m_lpBase.getRows(); rows.first != rows.second; ++rows.first) {
                RowId row = *rows.first;
                if (m_lpBase.getRowBoundType(row) != FR) {
                    if(call<RelaxCondition>(m_problem, m_lpBase, row)) {
                        ++deleted;
                        m_visitor.relaxRow(m_problem, m_lpBase, row);
                        call<DeleteRowStrategy>(m_lpBase, row);
                        repeat = true;
                        break;
                    }
                }
            }
        }
        
        return deleted > 0;
    }

    LPBase & getLP() {
        return m_lpBase;
    }
    
    IRComponents & getIRComponents() {
        return m_irComponents;
    }

    typedef decltype(std::bind(&IterativeRounding::getVal, std::declval<const IterativeRounding *>(), std::placeholders::_1)) GetSolution;

    decltype(std::declval<IRComponents>().template call<SetSolution>(std::declval<Problem &>(), std::declval<GetSolution &>())) setSolution() {
        call<SetSolution>(m_problem, std::bind(&IterativeRounding::getVal, this, std::placeholders::_1));
    }

    void dependentRound() {
         call<RoundCondition>(m_problem, m_lpBase);
    }

    bool stopCondition() {
        return call<StopCondition>(m_problem);
    }

private:   
    template <typename Action, typename... Args> 
    auto call(Args&&... args) -> 
    decltype(std::declval<IRComponents>().template call<Action>(args...)){
        return m_irComponents.template call<Action>(args...);
    }
    
    LPBase m_lpBase;
    IRComponents m_irComponents;
    Visitor m_visitor;
    utils::Compare<double> m_compare;
    std::unordered_map<int, double> m_rounded;
    Problem & m_problem;
};

//TODO add resolve
template <typename Problem, typename IRComponents, typename Visitor = TrivialVisitor, typename LPBase = GLP>
ProblemType solve_iterative_rounding(Problem & problem, IRComponents components, Visitor vis = Visitor()) {
    IterativeRounding<Problem, IRComponents, Visitor, LPBase> ir(problem, std::move(components), std::move(vis));
    bool irSolved = false;
    do {
        auto probType = ir.solveLPToExtremePoint();
        if (probType != OPTIMAL) {
            return probType;
        }
        bool rounded{ir.round()};
        bool relaxed{ir.relax()};
        irSolved = ir.integerSolution();
        assert(irSolved || rounded || relaxed);
    } while (!irSolved);
    ir.setSolution();
    return OPTIMAL;
}

template <typename Solution, typename IRComponents, typename Visitor = TrivialVisitor, typename LPBase = GLP>
ProblemType solve_dependent_iterative_rounding(Solution& solution, IRComponents components, Visitor vis = Visitor()) {
    IterativeRounding<Solution, IRComponents, Visitor, LPBase> ir(solution, std::move(components), std::move(vis));
    do {
        auto probType = ir.solveLPToExtremePoint();
        if (probType != OPTIMAL) {
            return probType;
        }
        ir.dependentRound();
        ir.relax();
        if (ir.stopCondition()) break;
    } while (true);
    ir.setSolution();
    return OPTIMAL;
}

} //ir
} //paal


#endif //ITERATIVE_ROUNDING_HPP 
