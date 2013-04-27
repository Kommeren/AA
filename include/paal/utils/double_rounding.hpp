/**
 * @file double_rounding.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-04-25
 */
#ifndef DOUBLE_ROUNDING_HPP
#define DOUBLE_ROUNDING_HPP 
namespace paal {
namespace utils {
template <typename T>
class Compare {
public:
    static bool e(T a, T b) {
        return std::abs(a -b ) < EPSILON;
    }
    
    static bool ge(T a, T b) {
        return a >= b - EPSILON;
    }

private:
    static const T EPSILON;
};

template <typename T> const T Compare<T>::EPSILON = std::numeric_limits<T>::epsilon();

}
}
#endif /* DOUBLE_ROUNDING_HPP */
