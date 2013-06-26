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
    Compare(T epsilon = std::numeric_limits<T>::epsilon()): m_epsilon(epsilon) {}
    
    bool e(T a, T b) const {
        return std::abs(a - b) < m_epsilon;
        //return abs(a -b ) < m_epsilon; //this line breaks generalised_assignment_long_test TODO investigate
    }
    
    bool g(T a, T b) const {
        return a > b + m_epsilon;
    }
    
    bool ge(T a, T b) const {
        return a >= b - m_epsilon;
    }
    
    bool le(T a, T b) const {
        return a <= b + m_epsilon;
    }
    
    static T defaultEpsilon() {
        return std::numeric_limits<T>::epsilon();
    }

private:
    const T m_epsilon;
};

}
}
#endif /* DOUBLE_ROUNDING_HPP */
