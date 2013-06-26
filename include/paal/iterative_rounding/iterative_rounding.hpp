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
    void roundCol(LP &lp, int col, double val) {}
    
    template <typename LP>
    void relaxRow(LP &lp, int row) {}
};

template <typename Engine, typename Visitor = TrivialVisitor, typename LPBase = GLP>
class IterativeRounding  {
public:
    IterativeRounding(Engine e) : m_engine(std::move(e)) {
        m_engine.init(m_lpBase);
    }
    
    double solve() {
        return m_engine.solveLP(m_lpBase);
    }

    bool integerSolution() {
        int size = m_lpBase.colSize();
        for(int col = 1; col <= size; ++col) {
            double colVal = m_lpBase.getColPrim(col);
            if (!m_compare.e(colVal, std::round(colVal))) {
                return false;
            }
        }
        
        return true;
    }
    
    bool round() {
        int deleted(0);
        int size = m_lpBase.colSize();
        for(int i = 1; i <= size; ++i) {
            auto doRound = m_engine.roundCondition(m_lpBase, i);
            if(doRound.first) {
                ++deleted;
                m_visitor.roundCol(m_lpBase, i, doRound.second);
                roundColToValue(i, doRound.second);
                --size;
                --i;
            }
        }
        
        return deleted > 0;
    }
    
    bool relax() {
        int deleted(0);
        int size = m_lpBase.rowSize();
        for(int i = 1; i <= size; ++i) {
            if(m_engine.relaxCondition(m_lpBase, i)) {
                ++deleted;
                m_visitor.relaxRow(m_lpBase, i);
                m_lpBase.deleteRow(i);
                --size;
                --i;
            }
        }
        
        return deleted > 0;
    }

    LPBase & getLP() {
        return m_lpBase;
    }
    
    Engine & getEngine() {
        return m_engine;
    }

    decltype(std::declval<Engine>().getSolution(std::declval<LPBase>())) getSolution() {
        return m_engine.getSolution(m_lpBase);
    }


private:    
   
    void roundColToValue(int col, double value) {
        auto column = m_lpBase.getColumn(col);
        int row;
        double coef;
        for(const boost::tuple<int, double> & c : utils::make_range(column)) {
            boost::tie(row, coef) = c;
            double currUb = m_lpBase.getRowUb(row);
            double currLb = m_lpBase.getRowLb(row);
            BoundType currType = m_lpBase.getRowBoundType(row);
            double diff = coef * value;
            m_lpBase.setRowBounds(row, currType, currLb - diff, currUb - diff);
        }
        m_lpBase.deleteCol(col);
    }
    
    LPBase m_lpBase;
    Engine m_engine;
    Visitor m_visitor;
    utils::Compare<double> m_compare;
};

template <typename IR> 
void solve(IR &ir) {
    do {
        ir.solve();
        bool rounded = ir.round();
        bool relaxed = ir.relax();
        assert(rounded || relaxed);
    } while (!ir.integerSolution());
}

} //ir
} //paal


#endif //ITERATIVE_ROUNDING_HPP 
