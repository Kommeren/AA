/**
 * @file iterator_with_stop_condition_test.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-06-20
 */


#include "paal/utils/iterator_with_stop_condition.hpp"
#include "paal/local_search/custom_components.hpp"

#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(iterator_with_stop_conditionTest) {
    std::vector<int> v{1,2,3,4,5,6,7,8};
    paal::local_search::stop_condition_count_limit stopcondition(3);

    auto begin = paal::make_beginiterator_with_stop_condition(
                    v.begin(), v.end(), stopcondition);

    auto end = paal::make_enditerator_with_stop_condition(
                    v.end(), stopcondition);

    BOOST_CHECK(begin != end);
    BOOST_CHECK_EQUAL(*begin, 1);
    ++begin;
    BOOST_CHECK(begin != end);
    BOOST_CHECK_EQUAL(*begin, 2);
    begin += 3;
    BOOST_CHECK(begin != end);
    BOOST_CHECK_EQUAL(*begin, 5);
    ++begin;
    BOOST_CHECK(begin == end);
}


