/**
 * @file knapsack_utils.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-10-07
 */
#ifndef KNAPSACK_UTILS_HPP
#define KNAPSACK_UTILS_HPP 

#include "paal/utils/type_functions.hpp"

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
    
template <typename ObjectsIter,
           typename ObjectSizeFunctor, 
           typename ObjectValueFunctor>
 puretype(std::declval<ObjectValueFunctor>()(*std::declval<ObjectsIter>()))
 getDensityValueBound(ObjectsIter oBegin, ObjectsIter oEnd, 
  puretype(std::declval<ObjectSizeFunctor>()(*std::declval<ObjectsIter>())) capacity,
  ObjectValueFunctor value, ObjectSizeFunctor size) {
      typedef typename std::iterator_traits<ObjectsIter>::reference  ObjectRef;

      auto density = make_Density(value, size);
      auto maxElement = density(*std::max_element(oBegin, oEnd, 
                       [&](ObjectRef left, ObjectRef right){return density(left) < density(right);}));
      return capacity * maxElement;
}

}
#endif /* KNAPSACK_UTILS_HPP */
