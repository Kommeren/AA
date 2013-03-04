/**
 * @file local_search_obj_fun_test.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-11
 */

#define BOOST_TEST_MODULE local_search_obj_fun_test

#include <boost/test/unit_test.hpp>

#include <vector>
#include <string>

#include "paal/local_search/local_search_step.hpp"
#include "utils/logger.hpp"

//#define VERBOSE 

using std::string;
using std::vector;
using namespace  paal::local_search;
using namespace  paal;

struct F {
    int operator()(int x) {
        return -x*x + 12 * x -27;
    }
};

struct NG {
    typedef typename std::vector<int>::const_iterator Iter;
    const std::vector<int> neighb;
public:

    NG() : neighb{10, -10, 1, -1} {}

    std::pair<Iter, Iter> operator()(int x) const {
        return std::make_pair(neighb.begin(), neighb.end());
    }
};


struct SU {
    void operator()(int & s, int u) const {
        s = s + u;
    }
};


BOOST_AUTO_TEST_CASE(local_search_obj_fun_test) {
   //creating local search
   typedef  SearchObjFunctionComponents<NG, F, SU> SearchComp;
   LocalSearchFunctionStep<int, SearchComp> ls;
#ifdef LOGGER_ON
   F f;
#endif

   //printing 
   auto const & s = ls.getSolution();
   LOG("f("<< s <<") \t" << f(s));

#ifdef LOGGER_ON
   int i = 0;
#endif

   //search
   while(ls.search()) {
       //printing
       LOG("f("<< s <<") \t" << f(s)  << " after " << ++i);
   }
   BOOST_CHECK_EQUAL(s, 6);
}


