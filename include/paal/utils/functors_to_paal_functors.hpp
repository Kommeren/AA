/**
 * @file functors_to_paal_functors.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-11
 */
#ifndef FUNCTORS_TO_PAAL_FUNCTORS_HPP
#define FUNCTORS_TO_PAAL_FUNCTORS_HPP 

#include <utility>

namespace paal {
namespace utils {

#define FUNCT_TO(className, memberName) \
template <typename F> class FunctTo##className {\
    public:\
        FunctTo##className(const F & f) : m_f(f) {}\
        template <typename... Args> auto  \
            memberName(Args&&... args) \
                -> decltype(std::declval<F>()(std::forward<Args>(args)...)) {\
            return m_f(std::forward<Args>(args)...);\
        }\
    private:\
        const F & m_f;\
};\


FUNCT_TO(GetNeighborhood, get)
FUNCT_TO(Gain, gain)
FUNCT_TO(UpdateSolution, update)
FUNCT_TO(StopCondition, stop)

}
}

#endif /* FUNCTORS_TO_PAAL_FUNCTORS_HPP */
