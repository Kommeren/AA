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

#include <boost/test/unit_test.hpp>



BOOST_AUTO_TEST_CASE(splay_cycle_test) {
    std::vector<int> v{1,2,3,4,5,6,7,8,9,10};
    paal::data_structures::splay_cycle<int> sc(v.begin(), v.end());
    auto b = sc.vbegin();
    auto e = sc.vend();
    BOOST_CHECK(vec_equals(b,e, v.begin(), v.end()));
    LOG_COPY_DEL(b, e, ",");
    b = sc.vbegin(3);
    std::vector<int> v1{3,4,5,6,7,8,9,10,1,2};
    BOOST_CHECK(vec_equals(b,e, v1.begin(), v1.end()));
    LOG_COPY_DEL(b, e, ",");
    sc.flip(3, 5);
    std::vector<int> v2{1,2,5,4,3,6,7,8,9,10};
    BOOST_CHECK(vec_equals(sc.vbegin(), sc.vend(), v2.begin(), v2.end()));
    LOG_COPY_DEL(sc.vbegin(), sc.vend(), ",");
    sc.flip(8, 2);
    std::vector<int> v3{10, 9, 8, 5, 4, 3, 6, 7,2 ,1};
    BOOST_CHECK(vec_equals(sc.vbegin(), sc.vend(), v3.begin(), v3.end()));
    LOG_COPY_DEL(sc.vbegin(), sc.vend(), ",");
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

