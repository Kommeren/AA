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
#include <string>
#include <map>
#include <set>

#include <boost/range/irange.hpp>

#include "paal/utils/type_functions.hpp"
#include "paal/utils/iterator_utils.hpp"
#include "paal/utils/double_rounding.hpp"
#include "paal/iterative_rounding/glp_lpbase.hpp"


namespace paal {
namespace ir {

struct TrivialVisitor {
    template <typename LP>
    void roundCol(LP &lp, ColId col, double val) {}
    
    template <typename LP>
    void relaxRow(LP &lp, RowId row) {}
};

template <typename IRComponents, typename Visitor = TrivialVisitor, typename LPBase = GLP>
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
    IterativeRounding(IRComponents e) : m_irComponents(std::move(e)) {
        m_irComponents.init(m_lpBase);
    }
    
    double solve() {
        return m_irComponents.solveLP(m_lpBase);
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
                    auto doRound = m_irComponents.roundCondition(m_lpBase, col);
                    if(doRound.first) {
                        m_rounded.insert(std::make_pair(col.get(), doRound.second));
                        ++deleted;
                        m_visitor.roundCol(m_lpBase, col, doRound.second);
                        m_irComponents.deleteCol(m_lpBase, col, doRound.second);
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
                    if(m_irComponents.relaxCondition(m_lpBase, row)) {
                        ++deleted;
                        m_visitor.relaxRow(m_lpBase, row);
                        m_irComponents.deleteRow(m_lpBase, row);
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

    decltype(std::declval<IRComponents>().getSolution(std::declval<GetSolution &>())) getSolution() {
        return m_irComponents.getSolution(std::bind(&IterativeRounding::getVal, this, std::placeholders::_1));
    }


private:   
    
    LPBase m_lpBase;
    IRComponents m_irComponents;
    Visitor m_visitor;
    utils::Compare<double> m_compare;
    std::unordered_map<int, double> m_rounded;
};

template <typename IR> 
void solve(IR &ir) {
    bool irsol=false;
    do {
        ir.solve();
        bool rounded = ir.round();
        bool relaxed = ir.relax();
        irsol=ir.integerSolution();
        assert( irsol || rounded || relaxed);
    } while (!irsol);
}

} //ir
} //paal


#endif //ITERATIVE_ROUNDING_HPP 
