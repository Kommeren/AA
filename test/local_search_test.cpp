/**
 * @file local_search.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-04
 */

#define BOOST_TEST_MODULE local_search_test

#include <boost/test/unit_test.hpp>

#include <vector>
#include <string>

#include "local_search/local_search.hpp"

using std::string;
using std::vector;
using namespace  paal::local_search;
using namespace  paal;

int f(int x) {
    return -x*x + 12 * x -27;
}

struct NG {
    typedef typename std::vector<int>::const_iterator Iter;
    const std::vector<int> neighb;
public:

    NG() : neighb{10, -10, 1, -1} {}

    std::pair<Iter, Iter> get(int x) {
        return std::make_pair(neighb.begin(), neighb.end());
    }
};

struct CII {
    int gain(int s, int u) {
        return f(s + u) - f(s);
    }
};

struct SU {
    void update(int & s, int u) {
        s = s + u;
    }
};


BOOST_AUTO_TEST_CASE(two_local_search_test) {
   //creating local search
   LocalSearchStep<int, NG, CII, SU> ls(0, NG(), CII(), SU());

   //printing 
   auto const & s = ls.getSolution();
   std::cout << "f("<< s <<") \t" << f(s)  << std::endl;
   int i = 0;

   //search
   while(ls.search()) {
       //printing
       std::cout << "f("<< s <<") \t" << f(s)  << " after " << ++i << std::endl;
   }
   BOOST_CHECK_EQUAL(s, 6);
}

