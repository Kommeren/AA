/**
 * @file cycle.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-03-11
 */
#ifndef CYCLE_HPP
#define CYCLE_HPP

#include "paal/data_structures/cycle/simple_cycle.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/range/algorithm/equal.hpp>

#include <vector>
#include <string>


namespace {
std::vector<std::string> v = { "1", "2", "3", "4", "5", "6", "7", "8", "9",
                               "10" };
}

template <typename El, typename Sol, typename Cycle>
void check_swap(Cycle &c, const El &p1, const El &p2, const El &start,
                const Sol &sol) {
    c.flip(p1, p2);
    BOOST_CHECK(boost::equal(sol, boost::make_iterator_range(c.vbegin(start), c.vend())));
}

template <template <class> class Cycle> void swap_edges_3() {
    std::vector<std::string> sol = { "1", "2", "3" };
    Cycle<std::string> sc(v.begin(), v.begin() + 3);
    check_swap(sc, "2", "2", "1", sol);
}

template <template <class> class Cycle> void swap_edges_3_1() {
    std::vector<std::string> sol = { "1", "3", "2" };
    Cycle<std::string> sc(v.begin(), v.begin() + 3);
    check_swap(sc, "3", "1", "1", sol);
}

template <template <class> class Cycle> void swap_edges_4() {
    std::vector<std::string> sol = { "1", "3", "2", "4" };

    Cycle<std::string> sc(v.begin(), v.begin() + 4);
    check_swap(sc, "2", "3", "1", sol);
}
#endif /* CYCLE_HPP */
