/**
 * @file knapsack_utils.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-10-07
 */
#ifndef KNAPSACK_UTILS_HPP
#define KNAPSACK_UTILS_HPP 

namespace paal {

template <typename Value, typename Size>
struct Density {

    Density(Value value, Size size) : m_value(value), m_size(size) {}

    template <typename ObjectRef>
    double operator()(ObjectRef obj) const {
        return double(m_value(obj)) / double(m_size(obj));
    }
private:
    Value m_value;
    Size m_size;
};

template <typename Value, typename Size>
Density<Value, Size>
make_Density(Value value, Size size) {
    return Density<Value, Size>(value, size);
}

}
#endif /* KNAPSACK_UTILS_HPP */
