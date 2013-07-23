/**
 * @file local_search_test.cpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-04
 */

#define BOOST_TEST_MODULE local_search_test

#include <boost/test/unit_test.hpp>

#include <vector>

#include "paal/local_search/local_search_step.hpp"
#include "utils/logger.hpp"

namespace ls = paal::local_search;
using namespace  paal;

int f(int x) {
    return -x*x + 12 * x -27;
}

struct GetNeigh {
    typedef typename std::vector<int>::const_iterator Iter;
    const std::vector<int> neighb;
public:

    GetNeigh() : neighb{10, -10, 1, -1} {}

    std::pair<Iter, Iter> operator()(int x) {
        return std::make_pair(neighb.begin(), neighb.end());
    }
};

struct Gain {
    int operator()(int s, int u) {
        return f(s + u) - f(s);
    }
};

struct UpdateSolution {
    void operator()(int & s, int u) {
        s = s + u;
    }
};

typedef  ls::SearchComponents<GetNeigh, Gain, UpdateSolution> SearchComp;
   
ON_LOG(int i = 0);
auto logAction = [&](int s) {
   LOG("f("<< s <<") \t" << f(s)  << " after " << ++i);
};


BOOST_AUTO_TEST_CASE(two_local_search_choose_first_better_test) {
   //creating local search
    ls::LocalSearchStep<int, ls::search_strategies::ChooseFirstBetter, SearchComp> ls;

   //printing
   int & solution = ls.getSolution();
   LOG("f("<< solution <<") \t" << f(solution));

   //search
   BOOST_CHECK(search(ls, logAction));
   BOOST_CHECK_EQUAL(solution, 6);
}


BOOST_AUTO_TEST_CASE(two_local_search_steepest_slope_test) {
   //creating local search
    ls::LocalSearchStep<int, ls::search_strategies::SteepestSlope, SearchComp> ls;

   //printing 
   auto const & s = ls.getSolution();
   LOG("f("<< s <<") \t" << f(s));
   ON_LOG(i = 0);
   
   //search
   BOOST_CHECK(search(ls, logAction));

   BOOST_CHECK_EQUAL(s, 6);
}
