/**
 * @file splay_cycle.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-04
 */

#include "utils/logger.hpp"
#include "cycle.hpp"

#include "paal/data_structures/cycle/splay_cycle.hpp"
#include "paal/data_structures/cycle/cycle_algo.hpp"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(splay_cycle_test) {
    std::vector<int> v{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    paal::data_structures::splay_cycle<int> sc(v.begin(), v.end());
    auto construct_range = [&]() {
        return boost::make_iterator_range(sc.vbegin(), sc.vend());
    };

    BOOST_CHECK(boost::equal(construct_range(), v));
    LOG_COPY_RANGE_DEL(construct_range(), ",");
    LOGLN("");
    ON_LOG(paal::simple_algo::print(sc, std::cout));

    auto range_rotated = boost::make_iterator_range(sc.vbegin(3), sc.vend());
    BOOST_CHECK(
        boost::equal(range_rotated,
                     std::array<int, 10>{ { 3, 4, 5, 6, 7, 8, 9, 10, 1, 2 } }));
    LOG_COPY_RANGE_DEL(range_rotated, ",");
    LOGLN("");

    sc.flip(3, 5);
    std::vector<int> v2{ 1, 2, 5, 4, 3, 6, 7, 8, 9, 10 };
    BOOST_CHECK(boost::equal(construct_range(), v2));
    LOG_COPY_RANGE_DEL(construct_range(), ",");
    LOGLN("");

    sc.flip(8, 2);
    std::vector<int> v3{ 10, 9, 8, 5, 4, 3, 6, 7, 2, 1 };
    BOOST_CHECK(boost::equal(construct_range(), v3));
    LOG_COPY_RANGE_DEL(construct_range(), ",");
    LOGLN("");

    paal::data_structures::splay_cycle<int> sc2(sc);
}

BOOST_AUTO_TEST_CASE(swap_edges_3_test) {
    swap_edges_3<paal::data_structures::splay_cycle>();
}

BOOST_AUTO_TEST_CASE(swap_edges_3_1_test) {
    swap_edges_3_1<paal::data_structures::splay_cycle>();
}

BOOST_AUTO_TEST_CASE(swap_edges_4_test) {
    swap_edges_4<paal::data_structures::splay_cycle>();
}
