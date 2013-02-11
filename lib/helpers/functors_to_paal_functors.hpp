/**
 * @file functors_to_paal_functors.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-11
 */
#ifndef FUNCTORS_TO_PAAL_FUNCTORS_HPP
#define FUNCTORS_TO_PAAL_FUNCTORS_HPP 

//TODO write makro
template <typename F> class FunctToNeigh {
    public:
        FunctToNeigh(const F & f) : m_f(f) {}
        template <typename Solution> auto get(const Solution &s) -> decltype(std::declval<F>()(s)) {
            return m_f(s);
        }
    private:
        const F & m_f;
};

#endif /* FUNCTORS_TO_PAAL_FUNCTORS_HPP */
