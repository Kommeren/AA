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

#include <boost/optional.hpp>
#include <boost/tuple/tuple.hpp>

#include "paal/utils/iterator_utils.hpp"
#include "paal/utils/double_rounding.hpp"
#include "paal/utils/do_nothing_functor.hpp"

#include "paal/data_structures/components/components.hpp"
#include "paal/lp/ids.hpp"
#include "paal/lp/bound_type.hpp"

namespace paal {
namespace ir {

class DefaultRoundCondition {
public:
    DefaultRoundCondition(double epsilon = utils::Compare<double>::defaultEpsilon()): m_compare(epsilon) { }
  
    template <typename Solution, typename LP>
    boost::optional<double> operator()(Solution &, const LP & lp, ColId col) {
        double x = lp.getColPrim(col);
        double r = std::round(x);
        if(m_compare.e(x,r)) {
            return r;
        }
        return boost::optional<double>();
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
    
    template <typename Solution, typename LP>
    boost::optional<double> operator()(Solution &, const LP & lp, ColId col) {
        return get(lp, lp.getColPrim(col));
    }
protected:
    template <typename LP>
    boost::optional<double> get(const LP & lp, double x) {
        if(this->m_compare.e(x, arg)) {
            return double(arg);
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
    boost::optional<double> get(const LP & lp, double x) {
        return boost::optional<double>();
    }
    
    const utils::Compare<double> m_compare;
};
        

template <typename Cond, typename F> 
class RoundConditionToFun {
public:
    RoundConditionToFun(Cond c = Cond(), F f = F()) : 
        m_cond(c), m_f(f) {}

    template <typename Solution, typename LP>
    boost::optional<double> operator()(Solution &, const LP & lp, ColId col) {
        double x = lp.getColPrim(col);
        if(m_cond(x)) {
            return m_f(x);
        }
        return boost::optional<double>();
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
    public RoundConditionToFun<CondBiggerEqualThan, utils::ReturnSomethingFunctor<int, 1> > {
        RoundConditionGreaterThanHalf(double epsilon = utils::Compare<double>::defaultEpsilon()) : 
            RoundConditionToFun(CondBiggerEqualThan(0.5, epsilon)) {}
};


struct DefaultSolveLPToExtremePoint {
    template <typename Solution, typename LP>
    double operator()(Solution &, LP & lp) {
        return lp.solveToExtremePoint();
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
        for(auto const & c : utils::make_range(column)) {
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

class SolveLPToExtremePoint;
class RoundCondition;
class RelaxCondition;
class Init;
class SetSolution;
class DeleteRowStrategy;
class DeleteColStrategy;

typedef data_structures::Components<   
        data_structures::NameWithDefault<SolveLPToExtremePoint, DefaultSolveLPToExtremePoint>,
        data_structures::NameWithDefault<RoundCondition, DefaultRoundCondition>,
        data_structures::NameWithDefault<RelaxCondition, utils::ReturnFalseFunctor>,
        data_structures::NameWithDefault<Init, utils::DoNothingFunctor>,
        data_structures::NameWithDefault<SetSolution, utils::DoNothingFunctor>,
        data_structures::NameWithDefault<DeleteRowStrategy, DeleteRow>,
        data_structures::NameWithDefault<DeleteColStrategy, DeleteCol>> Components;
        
template <typename... Args>
    using IRComponents = typename Components::type<Args...> ;

template <typename... Args>
auto make_IRComponents(Args&&... args) -> decltype(Components::make_components(std::forward<Args>(args)...)) {
      return Components::make_components(std::forward<Args>(args)...);
}

} //ir
} //paal


#endif /* IR_COMPONENTS_HPP */
