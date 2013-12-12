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
/**
 * @brief compare pointee using comparator
 *
 * @tparam Comparator
 */
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

/**
 * @brief  make function for less_pointees_t
 *
 * @tparam Comparator
 * @param compare
 *
 * @return 
 */
template<class Comparator>
less_pointees_t<Comparator>
make_less_pointees_t(Comparator compare) {
    return less_pointees_t<Comparator>(compare);
}


} //paal
#endif /* LESS_POINTEES_HPP */
