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

template <typename T, T t>
struct ReturnSomethingFunctor {
    template <typename ... Args > 
    T  operator()(Args&&... args) const {
        return t;
    } 
};

struct ReturnFalseFunctor : 
    public ReturnSomethingFunctor<bool, false> {};

struct ReturnTrueFunctor : 
    public ReturnSomethingFunctor<bool, true> {};

struct ReturnZeroFunctor :
    public ReturnSomethingFunctor<int, 0> {};
}
}
#endif /* DO_NOTIHNG_FUNCTOR_HPP */
