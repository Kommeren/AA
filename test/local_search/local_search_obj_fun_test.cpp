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

#include "paal/local_search/single_solution_step/search_obj_func_components.hpp"
#include "paal/local_search/single_solution_step/local_search_single_solution_obj_function.hpp"
#include "utils/logger.hpp"


using std::string;
using std::vector;
namespace ls = paal::local_search;
using namespace  paal;

struct F {
    int operator()(int x) {
        return -x*x + 12 * x -27;
    }
};

class NG {
    typedef const std::vector<int> Neighb;
    Neighb neighb;
    typedef typename Neighb::const_iterator Iter;
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
    typedef  ls::SearchComponentsObjFun<NG, F, SU> SearchComp;
    ON_LOG(F f);

    //printing 
    int s(0);
    LOG("f("<< s <<") \t" << f(s));
    ON_LOG(int i = 0);

    //setting logger
    auto logger =  [&](int s) {
        //printing
        LOG("f("<< s <<") \t" << f(s)  << " after " << ++i);
        };

    //search
    ls::local_search_obj_fun(s, logger, utils::ReturnFalseFunctor(), SearchComp()); 
    BOOST_CHECK_EQUAL(s, 6);
}
