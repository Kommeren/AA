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
    static bool e(T a, T b, T epsilon = EPSILON) {
        return std::abs(a - b) < epsilon;
        //return abs(a -b ) < EPSILON; //this line breaks generalised_assignment_long_test TODO investigate
    }
    
    static bool ge(T a, T b, T epsilon = EPSILON) {
        return a >= b - epsilon;
    }
    
    static bool le(T a, T b, T epsilon = EPSILON) {
        return a <= b + epsilon;
    }
    
    static T epsilon() {
        return EPSILON;
    }

private:
    static const T EPSILON;
};

template <typename T> const T Compare<T>::EPSILON = std::numeric_limits<T>::epsilon();

}
}
#endif /* DOUBLE_ROUNDING_HPP */
