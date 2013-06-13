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
    Array2Function(const Array & a, int offset = 0) : m_array(a), m_offset(offset) {}
    typedef decltype(std::declval<const Array>()[0]) Value;
    Value operator()(int a) const {return m_array[a + m_offset];}
private:
    const Array & m_array;
    int m_offset;
};

template <typename Array>
Array2Function<Array> make_Array2Function(const Array &a, int offset = 0) {
    return Array2Function<Array>(a, offset);
}
#endif /* ARRAY2FUNCTION_HPP */
