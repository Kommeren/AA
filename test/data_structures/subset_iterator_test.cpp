/**
 * @file subset_iterator_test.cpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-04
 */

#define BOOST_TEST_MODULE subset_iterator_test

#include <boost/test/unit_test.hpp>

#include <vector>
#include <string>

#include "paal/data_structures/subset_iterator.hpp"

using std::vector;

template <typename T> void printTuple3(T t) {
    LOGLN(std::get<0>(t) << "," << 
        std::get<1>(t) << "," << 
        std::get<2>(t));
}

typedef typename std::vector<int>::iterator It;
vector<int> v{1,2,3,4,5,6,7};

BOOST_AUTO_TEST_CASE(subset_iterator_test) {
    auto r =  paal::data_structures::make_SubsetsIteratorrange<It, 3>(v.begin(), v.end());
    typedef std::tuple<int, int, int> tup;
    BOOST_CHECK_EQUAL(std::get<0>(*r.first), 1);
    BOOST_CHECK_EQUAL(std::get<1>(*r.first), 2);
    BOOST_CHECK_EQUAL(std::get<2>(*r.first), 3);
    BOOST_CHECK_EQUAL(distance(r.first, r.second), 7*6*5/6);
}

void testDist(int i, int res) {
    auto r =  paal::data_structures::make_SubsetsIteratorrange<It, 3>(v.begin(), v.begin() + i);
    BOOST_CHECK_EQUAL(distance(r.first, r.second), res);
}

BOOST_AUTO_TEST_CASE(subset_iterator_corner_test) {
    testDist(6, 6*5*4/6);
    testDist(5, 5*4*3/6);
    testDist(4, 4);
    testDist(3, 1);
    testDist(2, 0);
    testDist(1, 0);
    testDist(0, 0);
}


