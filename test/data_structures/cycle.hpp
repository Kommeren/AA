/**
 * @file cycle.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-03-11
 */
#ifndef CYCLE_HPP
#define CYCLE_HPP

#include <boost/test/unit_test.hpp>
#include <vector>
#include <string>
#include "paal/data_structures/cycle/simple_cycle.hpp"

template <typename I1, typename I2> bool vecEquals(I1 b1, I1 e1, I2 b2, I2 e2) {
    if(std::distance(b1, e1) != std::distance(b2, e2)) {
        return false;
    }
    return std::equal(b1, e1 , b2);
}

namespace {
    std::vector<std::string> v = {"1","2","3","4","5","6","7","8","9","10"};
}

template <typename El, typename Sol, typename Cycle>
void checkSwap(Cycle & c, const El & p1, const  El & p2, const El & start, const Sol & sol) {
    c.flip(p1, p2);
    BOOST_CHECK(vecEquals(sol.begin(), sol.end(), c.vbegin(start), c.vend()));
}

template <template <class> class  Cycle>
void swap_edges_3() {
    std::vector<std::string> sol = {"1","2", "3"};
    Cycle<std::string> sc(v.begin(), v.begin() + 3);
    checkSwap(sc, "2", "2", "1", sol);
}

template <template <class> class  Cycle>
void swap_edges_3_1() {
    std::vector<std::string> sol = {"1", "3", "2"};
    Cycle<std::string> sc(v.begin(), v.begin() + 3);
    checkSwap(sc, "3", "1", "1", sol);
}

template <template <class> class  Cycle>
void swap_edges_4() {
    std::vector<std::string> sol = {"1","3", "2", "4"};

    Cycle<std::string> sc(v.begin(), v.begin() + 4);
    checkSwap(sc, "2", "3", "1", sol);
}
#endif /* CYCLE_HPP */
