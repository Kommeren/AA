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

#include "helpers/iterator_helpers.hpp"

using std::vector;

template <typename T> void printTuple3(T t) {
    std::cout  << std::get<0>(t) << ",";
    std::cout  << std::get<1>(t) << ",";
    std::cout  << std::get<2>(t) << std::endl;
}

BOOST_AUTO_TEST_CASE(subset_iterator_test) {
    vector<int> v{1,2,3,4,5,6,7};
    typedef paal::helpers::SubsetsIterator<typename std::vector<int>::iterator, 3> SubsetsIt;
    SubsetsIt b(v.begin(), v.end());
    SubsetsIt e(v.end(), v.end());
    BOOST_CHECK_EQUAL(distance(b,e), 7*6*5/6);
}

