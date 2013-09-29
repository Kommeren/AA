/**
 * @file less_pointees.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-09-29
 */
#ifndef LESS_POINTEES_HPP
#define LESS_POINTEES_HPP 
namespace paal {

//TODO add to boost
//compare pointee using comparator
template<class Comparator>
struct less_pointees_t {
    less_pointees_t(Comparator compare) : m_compare(compare) {}

    template <typename OptionalPointee> 
    bool operator() ( OptionalPointee const& x, OptionalPointee const& y ) const { 
       return !y ? false : ( !x ? true : m_compare(*x, *y) ) ;
    }
private:
    Comparator m_compare;
};

template<class Comparator>
less_pointees_t<Comparator>
make_less_pointees_t(Comparator compare) {
    return less_pointees_t<Comparator>(compare);
}


}
#endif /* LESS_POINTEES_HPP */
