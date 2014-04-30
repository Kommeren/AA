/**
 * @file subset_iterator_test.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-04
 */


#include "paal/data_structures/subset_iterator.hpp"

#include <boost/test/unit_test.hpp>

#include <vector>
#include <string>

using std::vector;

template <typename T> void print_tuple3(T t) {
    LOGLN(std::get<0>(t) << "," <<
        std::get<1>(t) << "," <<
        std::get<2>(t));
}

typedef typename std::vector<int>::iterator It;
vector<int> v{1,2,3,4,5,6,7};

BOOST_AUTO_TEST_CASE(subset_iterator_engine_test) {
    auto engine = paal::data_structures::make_subsets_iterator_engine<3>(v.begin(), v.begin() + 4);

    BOOST_CHECK(engine.call(paal::make_tuple{}) == std::make_tuple(1, 2, 3));
    BOOST_CHECK(engine.next());
    BOOST_CHECK(engine.call(paal::make_tuple{}) == std::make_tuple(1, 2, 4));
    BOOST_CHECK(engine.next());
    BOOST_CHECK(engine.call(paal::make_tuple{}) == std::make_tuple(1, 3, 4));
    BOOST_CHECK(engine.next());
    BOOST_CHECK(engine.call(paal::make_tuple{}) == std::make_tuple(2, 3, 4));
    BOOST_CHECK(!engine.next());
}

BOOST_AUTO_TEST_CASE(subset_iterator_test) {
    auto r =  paal::data_structures::make_subsets_iterator_range<3>(v.begin(), v.end());
    typedef std::tuple<int, int, int> tup;
    BOOST_CHECK_EQUAL(std::get<0>(*r.first), 1);
    BOOST_CHECK_EQUAL(std::get<1>(*r.first), 2);
    BOOST_CHECK_EQUAL(std::get<2>(*r.first), 3);
    BOOST_CHECK_EQUAL(distance(r.first, r.second), 7*6*5/6);
}

void test_dist(int i, int res) {
    auto r =  paal::data_structures::make_subsets_iterator_range<3>(v.begin(), v.begin() + i);
    BOOST_CHECK_EQUAL(distance(r.first, r.second), res);
}

BOOST_AUTO_TEST_CASE(subset_iterator_corner_test) {
    test_dist(6, 6*5*4/6);
    test_dist(5, 5*4*3/6);
    test_dist(4, 4);
    test_dist(3, 1);
    test_dist(2, 0);
    test_dist(1, 0);
    test_dist(0, 0);
}

