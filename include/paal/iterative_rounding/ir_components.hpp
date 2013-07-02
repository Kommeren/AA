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
    std::pair<bool, double> operator()(const LP & lp, ColId col) {
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
    std::pair<bool, double> operator()(const LP & lp, ColId col) {
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
        

template <typename Cond, typename F> 
class RoundConditionToFun {
public:
    RoundConditionToFun(Cond c = Cond(), F f = F()) : 
        m_cond(c), m_f(f) {}

    template <typename LP>
    std::pair<bool, double> operator()(const LP & lp, int col) {
        double x = lp.getColPrim(col);
        if(m_cond(x)) {
            return std::make_pair(true, m_f(x));
        }
        return std::make_pair(false, -1);
    }
private:
    Cond m_cond;
    F m_f;
};


class CondBiggerEqualThan {
public:
    CondBiggerEqualThan(double b, double epsilon = utils::Compare<double>::defaultEpsilon()) 
        : m_bound(b), m_compare(epsilon) {}    

    bool operator()(double x) {
        return m_compare.ge(x, m_bound);
    }

private:
    double m_bound;
    const utils::Compare<double> m_compare;
};


///A variable is rounded up to 1, if it has value at least half in the solution
struct RoundConditionGreaterThanHalf  : 
    public RoundConditionToFun<CondBiggerEqualThan, utils::ReturnSomethingFunctor<int, 1>> {
        RoundConditionGreaterThanHalf() : 
            RoundConditionToFun(CondBiggerEqualThan(0.5)) {}
};


struct DefaultSolveLP {
    template <typename LP>
    double operator()(LP & lp) {
        return lp.solve();
    };
};

struct DeleteRow {
    template <typename LP>
    void operator()(LP & lp, RowId row) {
        lp.deleteRow(row);
    };
};

struct DeleteCol {
    template <typename LP>
    void operator()(LP & lp, ColId col, double value) {
        auto column = lp.getRowsInColumn(col);
        RowId row;
        double coef;
        for(const boost::tuple<RowId, double> & c : utils::make_range(column)) {
            boost::tie(row, coef) = c;
            double currUb = lp.getRowUb(row);
            double currLb = lp.getRowLb(row);
            BoundType currType = lp.getRowBoundType(row);
            double diff = coef * value;
            lp.setRowBounds(row, currType, currLb - diff, currUb - diff);
        }
        lp.deleteCol(col);
    };
};

/* not supported now
struct TrivializeRow {
    template <typename LP>
    void operator()(LP & lp, RowId row) {
        lp.setRowBounds(row, FR, 0, 0);
    };
};

struct FixCol {
    template <typename LP>
    void operator()(LP & lp, ColId col, double value) {
        lp.setColBounds(col, FX, value, value);
    };
};*/

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
    std::pair<bool, double> roundCondition(const LP & lp, ColId col) {
        return m_roundCondition(lp, col);
    }

    template <typename LP>
    bool relaxCondition(const LP & lp, RowId row) {
        return m_relaxCondition(lp, row); 
    };
    
    template <typename LP>
    bool init(LP & lp) {
        return m_init(lp); 
    };
    
    template <typename LP>
    void deleteRow(LP & lp, RowId row) {
        return m_deleteRow(lp, row);
    };
    
    template <typename LP>
    void deleteCol(LP & lp, ColId col, double value) {
        return m_deleteCol(lp, col, value);
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
