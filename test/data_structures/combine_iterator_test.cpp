/**
 * @file combine_iterator_test.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-04
 */

#include "utils/logger.hpp"

#include "paal/data_structures/combine_iterator.hpp"
#include "paal/utils/make_tuple.hpp"

#include <boost/test/unit_test.hpp>

template <typename CombinedEngine>
void check_move(CombinedEngine &combined, int i, int j) {
    BOOST_CHECK(combined.next());
    BOOST_CHECK(combined.call(paal::make_tuple()) == std::make_tuple(i, j));
}

template <typename CombinedIter>
void check_plus(CombinedIter &iter, CombinedIter end, int i, double j, int k) {
    BOOST_CHECK(++iter != end);
    BOOST_CHECK(*iter == std::make_tuple(i, j, k));
}

BOOST_AUTO_TEST_CASE(combine_iterator_engineTest) {
    auto r1 = { 1, 2, 3, 4 };
    auto r2 = { 4, 5, 6, 7, 8 };
    auto combined = paal::data_structures::make_combine_iterator_engine(r1, r2);
    BOOST_CHECK(combined.call(paal::make_tuple()) == std::make_tuple(1, 4));
    check_move(combined, 1, 5);
    check_move(combined, 1, 6);
    check_move(combined, 1, 7);
    check_move(combined, 1, 8);
    check_move(combined, 2, 4);
    check_move(combined, 2, 5);
    check_move(combined, 2, 6);
    check_move(combined, 2, 7);
    check_move(combined, 2, 8);
    check_move(combined, 3, 4);
    check_move(combined, 3, 5);
    check_move(combined, 3, 6);
    check_move(combined, 3, 7);
    check_move(combined, 3, 8);
    check_move(combined, 4, 4);
    check_move(combined, 4, 5);
    check_move(combined, 4, 6);
    check_move(combined, 4, 7);
    check_move(combined, 4, 8);
    BOOST_CHECK(!combined.next());
}

BOOST_AUTO_TEST_CASE(combine_iteratorTest) {
    auto r1 = { 1, 2, 3, 4 };
    auto r2 = { 6., 7., 8. };
    auto r3 = { 9 };
    auto iter = paal::data_structures::make_combine_iterator(paal::make_tuple{},
                                                             r1, r2, r3);
    decltype(iter) end;
    BOOST_CHECK(iter != end);
    BOOST_CHECK(*iter == std::make_tuple(1, 6, 9));
    check_plus(iter, end, 1, 7., 9);
    check_plus(iter, end, 1, 8., 9);
    check_plus(iter, end, 2, 6., 9);
    check_plus(iter, end, 2, 7., 9);
    check_plus(iter, end, 2, 8., 9);
    check_plus(iter, end, 3, 6., 9);
    check_plus(iter, end, 3, 7., 9);
    check_plus(iter, end, 3, 8., 9);
    check_plus(iter, end, 4, 6., 9);
    check_plus(iter, end, 4, 7., 9);
    check_plus(iter, end, 4, 8., 9);
    BOOST_CHECK(++iter == end);
}

BOOST_AUTO_TEST_CASE(combine_iteratorEmptyTest) {
    auto r1 = { 1, 2, 3, 4 };
    auto r2 = std::vector<double>{};
    auto r3 = { 9 };
    auto iter = paal::data_structures::make_combine_iterator(paal::make_tuple{},
                                                             r1, r2, r3);
    decltype(iter) end;
    BOOST_CHECK(iter == end);
}

BOOST_AUTO_TEST_CASE(combine_iteratorEmpty2Test) {
    auto iter =
        paal::data_structures::make_combine_iterator(paal::make_tuple{});
    decltype(iter) end;
    BOOST_CHECK(iter == end);
}
