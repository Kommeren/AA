/**
 * @file do_notihng_functor.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-03-04
 */
#ifndef DO_NOTIHNG_FUNCTOR_HPP
#define DO_NOTIHNG_FUNCTOR_HPP 
namespace paal {
namespace utils {

struct DoNothingFunctor {
    template <typename ... Args > 
    void  operator()(Args&&... args) const {}
};

struct ReturnFalseFunctor {
    template <typename ... Args > 
    bool  operator()(Args&&... args) const {
        return false;
    } 
};

struct ReturnTrueFunctor {
    template <typename ... Args > 
    bool  operator()(Args&&... args) const {
        return true;
    }
};

}
}
#endif /* DO_NOTIHNG_FUNCTOR_HPP */
