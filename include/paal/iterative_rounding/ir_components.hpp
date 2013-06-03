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

struct DefaultRoundCondition {
    template <typename LP>
    std::pair<bool, double> operator()(const LP & lp, int col) {
        double x = lp.getColPrim(col);
        double r = std::round(x);
        if(utils::Compare<double>::e(x,r)) {
            return std::make_pair(true, r);
        }
        return std::make_pair(false, -1);
    };
};


template <int...> 
class RoundConditionEquals {
    RoundConditionEquals() = delete;
};

template <int arg, int... args> 
class RoundConditionEquals<arg, args...>  : 
        public RoundConditionEquals<args...> {
public:
    template <typename LP>
    std::pair<bool, double> operator()(const LP & lp, int col) {
        return get(lp, lp.getColPrim(col));
    }
protected:
    template <typename LP>
    std::pair<bool, double> get(const LP & lp, double x) {
        if(utils::Compare<double>::e(x, arg)) {
            return std::make_pair(true, arg);
        } else {
            return RoundConditionEquals<args...>::get(lp, x);
        }
    }
};

template <> 
class RoundConditionEquals<> { 
protected:
    template <typename LP>
    std::pair<bool, double> get(const LP & lp, double x) {
        return std::make_pair(false, -1);
    }
};



template <typename RoundCondition = DefaultRoundCondition, 
          typename RelaxContition = utils::ReturnFalseFunctor, 
          typename Init = utils::DoNothingFunctor> 
class IRComponents {
public:
    IRComponents(RoundCondition round = RoundCondition(), RelaxContition relax = RelaxContition(), Init i = Init()) 
        : m_roundCondition(round), m_relaxCondition(relax), m_init(i) {}


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

private:
    RoundCondition m_roundCondition;
    RelaxContition m_relaxCondition;
    Init m_init;
};

} //paal
} //ir


#endif /* IR_COMPONENTS_HPP */
