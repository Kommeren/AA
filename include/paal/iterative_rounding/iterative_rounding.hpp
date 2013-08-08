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
#include "paal/utils/iterator_utils.hpp"
#include "paal/utils/double_rounding.hpp"
#include "paal/iterative_rounding/glp_lpbase.hpp"
#include "paal/iterative_rounding/ir_components.hpp"


namespace paal {
namespace ir {

struct TrivialVisitor {
    template <typename Solution, typename LP>
    void roundCol(Solution & sol, LP &lp, ColId col, double val) {}
    
    template <typename Solution, typename LP>
    void relaxRow(Solution & sol, LP &lp, RowId row) {}
};

template <typename Solution, typename IRComponents, typename Visitor = TrivialVisitor, typename LPBase = GLP>
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
    IterativeRounding(Solution & sol, IRComponents e, Visitor vis = Visitor()) 
        : m_irComponents(std::move(e)), m_visitor(vis), m_solution(sol) {
        call<Init>(m_solution, m_lpBase);
    }
    
    double solveLPToExtremePoint() {
        return call<SolveLPToExtremePoint>(m_solution, m_lpBase);
    }

    bool integerSolution() {
        for(ColId col :utils::make_range(m_lpBase.getColumns())) {
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
                    auto doRound = call<RoundCondition>(m_solution, m_lpBase, col);
                    if(doRound) {
                        m_rounded.insert(std::make_pair(col.get(), *doRound));
                        ++deleted;
                        m_visitor.roundCol(m_solution, m_lpBase, col, *doRound);
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
                    if(call<RelaxCondition>(m_solution, m_lpBase, row)) {
                        ++deleted;
                        m_visitor.relaxRow(m_solution, m_lpBase, row);
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

    decltype(std::declval<IRComponents>().template call<SetSolution>(std::declval<Solution &>(), std::declval<GetSolution &>())) setSolution() {
        call<SetSolution>(m_solution, std::bind(&IterativeRounding::getVal, this, std::placeholders::_1));
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
    Solution & m_solution;
};



//TODO add resolve
template <typename Solution, typename IRComponents, typename Visitor = TrivialVisitor, typename LPBase = GLP>
void solve_iterative_rounding(Solution& solution, IRComponents components, Visitor vis = Visitor()) {
    IterativeRounding<Solution, IRComponents, Visitor, LPBase> ir(solution, components, vis);
    bool irsol=false;
    do {

        ir.solveLPToExtremePoint();
        bool rounded{ir.round()};
        bool relaxed{ir.relax()};
        irsol=ir.integerSolution();
        assert(irsol || rounded || relaxed);
    } while (!irsol);
    ir.setSolution();
}

} //ir
} //paal


#endif //ITERATIVE_ROUNDING_HPP 
