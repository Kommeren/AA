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

template <typename SolveLP = DefaultSolveLP, 
          typename RoundCondition = DefaultRoundCondition, 
          typename RelaxContition = utils::ReturnFalseFunctor, 
          typename Init = utils::DoNothingFunctor> 
class IRComponents {
public:
    IRComponents(SolveLP solve = SolveLP(), RoundCondition round = RoundCondition(),
                 RelaxContition relax = RelaxContition(), Init i = Init()) 
        : m_solveLP(solve), m_roundCondition(round), m_relaxCondition(relax), m_init(i) {}

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
    bool init(const LP & lp) {
        return m_init(lp); 
    };

protected:
    SolveLP m_solveLP;
private:
    RoundCondition m_roundCondition;
    RelaxContition m_relaxCondition;
    Init m_init;
};

} //paal
} //ir


#endif /* IR_COMPONENTS_HPP */
