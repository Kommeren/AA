/**
 * @file combine_iterator_test.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-04
 */

#include <boost/test/unit_test.hpp>
#define LOGGER_ON
#include "utils/logger.hpp"
#include "paal/data_structures/combine_iterator.hpp"
#include "paal/utils/make_tuple.hpp"

template <typename CombinedEngine>
void checkMove(CombinedEngine & combined, int i, int j) {
    BOOST_CHECK(combined.next());
    BOOST_CHECK(combined.call(paal::MakeTuple()) == std::make_tuple(i,j));
}

template <typename CombinedIter>
void checkPlus(CombinedIter & iter, CombinedIter end, int i, double j, int k) {
    BOOST_CHECK(++iter != end);
    BOOST_CHECK(*iter == std::make_tuple(i, j, k));
}


BOOST_AUTO_TEST_CASE(CombineIteratorEngineTest) {
    auto r1 = {1, 2, 3, 4};
    auto r2 = {4, 5, 6, 7, 8};
    auto combined = paal::data_structures::make_CombineIteratorEngine(r1, r2);
    BOOST_CHECK(combined.call(paal::MakeTuple()) == std::make_tuple(1,4));
    checkMove(combined, 1, 5);
    checkMove(combined, 1, 6);
    checkMove(combined, 1, 7);
    checkMove(combined, 1, 8);
    checkMove(combined, 2, 4);
    checkMove(combined, 2, 5);
    checkMove(combined, 2, 6);
    checkMove(combined, 2, 7);
    checkMove(combined, 2, 8);
    checkMove(combined, 3, 4);
    checkMove(combined, 3, 5);
    checkMove(combined, 3, 6);
    checkMove(combined, 3, 7);
    checkMove(combined, 3, 8);
    checkMove(combined, 4, 4);
    checkMove(combined, 4, 5);
    checkMove(combined, 4, 6);
    checkMove(combined, 4, 7);
    checkMove(combined, 4, 8);
    BOOST_CHECK(!combined.next());
}

BOOST_AUTO_TEST_CASE(CombineIteratorTest) {
    auto r1 = {1, 2, 3, 4};
    auto r2 = {6., 7., 8.};
    auto r3 = {9};
    auto iter = paal::data_structures::make_CombineIterator(
            paal::MakeTuple{},
            r1,
            r2,
            r3);
    decltype(iter) end;
    BOOST_CHECK(iter != end);
    BOOST_CHECK(*iter == std::make_tuple(1,6,9));
    checkPlus(iter, end, 1, 7., 9);
    checkPlus(iter, end, 1, 8., 9);
    checkPlus(iter, end, 2, 6., 9);
    checkPlus(iter, end, 2, 7., 9);
    checkPlus(iter, end, 2, 8., 9);
    checkPlus(iter, end, 3, 6., 9);
    checkPlus(iter, end, 3, 7., 9);
    checkPlus(iter, end, 3, 8., 9);
    checkPlus(iter, end, 4, 6., 9);
    checkPlus(iter, end, 4, 7., 9);
    checkPlus(iter, end, 4, 8., 9);
    BOOST_CHECK(++iter == end);
}

BOOST_AUTO_TEST_CASE(CombineIteratorEmptyTest) {
    auto r1 = {1, 2, 3, 4};
    auto r2 = std::vector<double>{};
    auto r3 = {9};
    auto iter = paal::data_structures::make_CombineIterator(
            paal::MakeTuple{}
            , r1, r2, r3);
    decltype(iter) end;
    BOOST_CHECK(iter == end);
}

BOOST_AUTO_TEST_CASE(CombineIteratorEmpty2Test) {
    auto iter = paal::data_structures::make_CombineIterator(
            paal::MakeTuple{});
    decltype(iter) end;
    BOOST_CHECK(iter == end);
}

