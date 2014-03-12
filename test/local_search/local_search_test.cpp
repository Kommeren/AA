/**
 * @file local_search_test.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-04
 */

#include <boost/test/unit_test.hpp>

#include <vector>

#include "utils/logger.hpp"
#include "utils/simple_single_local_search_components.hpp"

namespace ls = paal::local_search;
using namespace  paal;

namespace {
ON_LOG(
    int i = 0;
)

auto logAction = [&](int s) {
   LOGLN("f("<< s <<") \t" << f(s)  << " after " << ++i);
};

} //anonymous namespace
BOOST_AUTO_TEST_SUITE( local_search )

BOOST_AUTO_TEST_CASE(local_search_choose_first_better_test) {
   //printing
   int solution(0);
   LOGLN("f("<< solution <<") \t" << f(solution));
   utils::ReturnFalseFunctor nop;

   //search
   BOOST_CHECK(ls::local_search(solution, ls::ChooseFirstBetterStrategy{}, logAction, nop, SearchComp()));
   BOOST_CHECK_EQUAL(solution, 6);
}


BOOST_AUTO_TEST_CASE(local_search_steepest_slope_test) {
   //printing
   int s(0);
   LOGLN("f("<< s <<") \t" << f(s));
   ON_LOG(i = 0);
   utils::ReturnFalseFunctor nop;

   //search
   BOOST_CHECK(ls::local_search(s, ls::SteepestSlopeStrategy{},
                logAction, nop, SearchComp()));
   BOOST_CHECK_EQUAL(s, 6);
}
BOOST_AUTO_TEST_SUITE_END()

