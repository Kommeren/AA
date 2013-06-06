/**
 * @file array2function.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-06-06
 */
#ifndef ARRAY2FUNCTION_HPP
#define ARRAY2FUNCTION_HPP 
template <typename Array> 
class Array2Function{
public:
    Array2Function(const Array & a) : m_array(a) {}
    typedef decltype(std::declval<const Array>()[0]) Value;
    Value operator()(int a) const {return m_array[a];}
private:
    const Array & m_array;
};

template <typename Array>
Array2Function<Array> make_Array2Function(const Array &a) {
    return Array2Function<Array>(a);
}
#endif /* ARRAY2FUNCTION_HPP */
