/**
 * @file assign_updates.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2014-10-22
 */
#ifndef PAAL_LITERALS_HPP
#define PAAL_LITERALS_HPP

namespace paal {

/// removes boilerplate for x = max(x,y).
/// it becomes max_assign(x, y)
template <typename T>
void assign_max(T & t, const T & u) {
    t = std::max(t, u);
}

/// removes boilerplate for x = min(x,y).
/// it becomes min_assign(x, y)
template <typename T>
void assign_min(T & t, const T & u) {
    t = std::min(t, u);
}


} // ! paal

#endif // PAAL_LITERALS_HPP
