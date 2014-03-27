/**
 * @file local_search_obj_fun_test.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-11
 */

#include <boost/test/unit_test.hpp>

#include <vector>

#include "paal/local_search/search_obj_func_components.hpp"
#include "paal/local_search/local_search_obj_function.hpp"
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
public:

    NG() : neighb{10, -10, 1, -1} {}

    Neighb & operator()(int x) const {
        return neighb;
    }
};

struct SU {
    bool operator()(int & s, int u) const {
        s = s + u;
        return true;
    }
};

BOOST_AUTO_TEST_CASE(local_search_obj_fun_test) {
    //creating local search
    typedef  ls::search_componentsObjFun<NG, F, SU> search_comps;
    ON_LOG(F f);

    //printing
    int s(0);
    LOGLN("f("<< s <<") \t" << f(s));
    ON_LOG(int i = 0);

    //setting logger
    auto logger =  [&](int s) {
        //printing
        LOGLN("f("<< s <<") \t" << f(s)  << " after " << ++i);
        };

    //search
    ls::local_search_obj_fun(s, ls::choose_first_better_strategy{},
                logger, utils::return_false_functor(), search_comps());
    BOOST_CHECK_EQUAL(s, 6);
}
