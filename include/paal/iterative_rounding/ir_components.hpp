/**
 * @file ir_components.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-05-10
 */
#ifndef IR_COMPONENTS_HPP
#define IR_COMPONENTS_HPP 


#include <cmath>
#include "paal/utils/double_rounding.hpp"
#include "paal/utils/do_nothing_functor.hpp"

namespace paal {
namespace ir {
  

class DefaultRoundCondition {
public:
    DefaultRoundCondition(double epsilon = utils::Compare<double>::defaultEpsilon()): m_compare(epsilon) { }
  
    template <typename LP>
    std::pair<bool, double> operator()(const LP & lp, int col) {
        double x = lp.getColPrim(col);
        double r = std::round(x);
        if(m_compare.e(x,r)) {
            return std::make_pair(true, r);
        }
        return std::make_pair(false, -1);
    };
    
protected:
    const utils::Compare<double> m_compare;
};


template <int...> 
class RoundConditionEquals {
    RoundConditionEquals() = delete;
};

template <int arg, int... args> 
class RoundConditionEquals<arg, args...>  : 
        public RoundConditionEquals<args...> {
public:
    RoundConditionEquals(double epsilon = utils::Compare<double>::defaultEpsilon()): RoundConditionEquals<args...>(epsilon) { }
    
    template <typename LP>
    std::pair<bool, double> operator()(const LP & lp, int col) {
        return get(lp, lp.getColPrim(col));
    }
protected:
    template <typename LP>
    std::pair<bool, double> get(const LP & lp, double x) {
        if(this->m_compare.e(x, arg)) {
            return std::make_pair(true, arg);
        } else {
            return RoundConditionEquals<args...>::get(lp, x);
        }
    }
};

template <> 
class RoundConditionEquals<> {
public:
    RoundConditionEquals(double epsilon = utils::Compare<double>::defaultEpsilon()): m_compare(epsilon) { }
    
protected:
    template <typename LP>
    std::pair<bool, double> get(const LP & lp, double x) {
        return std::make_pair(false, -1);
    }
    
    const utils::Compare<double> m_compare;
};


struct DefaultSolveLP {
    template <typename LP>
    double operator()(LP & lp) {
        return lp.solve();
    };
};

struct DeleteRow {
    template <typename LP>
    void operator()(LP & lp, int & row, int & rowCnt) {
        lp.deleteRow(row);
        --row;
        --rowCnt;
    };
};

struct DeleteCol {
    template <typename LP>
    void operator()(LP & lp, int & col, double value, int & colCnt) {
        auto column = lp.getColumn(col);
        int row;
        double coef;
        for(const boost::tuple<int, double> & c : utils::make_range(column)) {
            boost::tie(row, coef) = c;
            double currUb = lp.getRowUb(row);
            double currLb = lp.getRowLb(row);
            BoundType currType = lp.getRowBoundType(row);
            double diff = coef * value;
            lp.setRowBounds(row, currType, currLb - diff, currUb - diff);
        }
        lp.deleteCol(col);
        --col;
        --colCnt;
    };
};

struct TrivializeRow {
    template <typename LP>
    void operator()(LP & lp, int & row, int & rowCnt) {
        lp.setRowBounds(row, FR, 0, 0);
    };
};

struct FixCol {
    template <typename LP>
    void operator()(LP & lp, int & col, double value, int & colCnt) {
        lp.setColBounds(col, FX, value, value);
    };
};

template <typename SolveLP = DefaultSolveLP, 
          typename RoundCondition = DefaultRoundCondition, 
          typename RelaxContition = utils::ReturnFalseFunctor, 
          typename Init = utils::DoNothingFunctor,
          typename DeleteRowStrategy = DeleteRow,
          typename DeleteColStrategy = DeleteCol> 
class IRComponents {
public:
    IRComponents(SolveLP solve = SolveLP(), RoundCondition round = RoundCondition(),
                 RelaxContition relax = RelaxContition(), Init i = Init(),
                 DeleteRowStrategy deleteRow = DeleteRowStrategy(), DeleteColStrategy deleteCol = DeleteColStrategy()) 
        : m_solveLP(solve), m_roundCondition(round), m_relaxCondition(relax), m_init(i), m_deleteRow(deleteRow), m_deleteCol(deleteCol) {}

    template <typename LP>
    double solveLP(LP & lp) {
        return m_solveLP(lp);
    }

    template <typename LP>
    std::pair<bool, double> roundCondition(const LP & lp, int col) {
        return m_roundCondition(lp, col);
    }

    template <typename LP>
    bool relaxCondition(const LP & lp, int row) {
        return m_relaxCondition(lp, row); 
    };
    
    template <typename LP>
    bool init(LP & lp) {
        return m_init(lp); 
    };
    
    template <typename LP>
    void deleteRow(LP & lp, int & row, int & rowCnt) {
        return m_deleteRow(lp, row, rowCnt);
    };
    
    template <typename LP>
    void deleteCol(LP & lp, int & col, double value, int & colCnt) {
        return m_deleteCol(lp, col, value, colCnt);
    };

private:
    SolveLP m_solveLP;
    RoundCondition m_roundCondition;
    RelaxContition m_relaxCondition;
    Init m_init;
    DeleteRowStrategy m_deleteRow;
    DeleteColStrategy m_deleteCol;
};

} //paal
} //ir


#endif /* IR_COMPONENTS_HPP */
