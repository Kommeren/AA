//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file local_search_obj_fun_test.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-11
 */
#include "test_utils/logger.hpp"

#include "paal/local_search/search_obj_func_components.hpp"
#include "paal/local_search/local_search_obj_function.hpp"
#include "paal/data_structures/components/components_replace.hpp"

#include <boost/test/unit_test.hpp>

#include <vector>

using std::string;
using std::vector;
namespace ls = paal::local_search;
using namespace paal;

struct F {
    int operator()(int x) const { return -x * x + 12 * x - 27; }
};

class NG {
    typedef const std::vector<int> Neighb;
    Neighb neighb;

  public:

    NG() : neighb{ 1, -1, 10, -10 } {}

    Neighb &operator()(int x) const { return neighb; }
};

struct SU {
    bool operator()(int &s, int u) const {
        s = s + u;
        return true;
    }
};

BOOST_AUTO_TEST_CASE(local_search_obj_fun_test) {
    // creating local search
    typedef ls::search_components_obj_fun<NG, F, SU> search_comps;
    ON_LOG(F f);

    // printing
    int s(0);
    LOGLN("f(" << s << ") \t" << f(s));
    ON_LOG(int i = 0);

    // setting logger
    auto logger = [&](int s) {
        // printing
        LOGLN("f(" << s << ") \t" << f(s) << " after " << ++i);
        return true;
    };

    ls::local_search_obj_fun(s, ls::first_improving_strategy{}, logger,
                             utils::always_false{}, search_comps{});
    BOOST_CHECK_EQUAL(s, 6);

    ON_LOG(i = 0);
    s = 0;
    ls::local_search_obj_fun(s, ls::best_improving_strategy{}, logger,
                             utils::always_false{}, search_comps{});
    BOOST_CHECK_EQUAL(s, 6);

    s = 0;
    ls::obj_fun_first_improving(s, search_comps{});
    BOOST_CHECK_EQUAL(s, 6);

    s = 0;
    ls::obj_fun_best_improving(s, search_comps{});
    BOOST_CHECK_EQUAL(s, 6);

    s = 0;
    ls::obj_fun_first_improving(s, search_comps{}, search_comps{});
    BOOST_CHECK_EQUAL(s, 6);

    s = 0;
    ls::obj_fun_best_improving(s, search_comps{}, search_comps{});
    BOOST_CHECK_EQUAL(s, 6);

    search_comps sc{};

    int iter = -1;

    auto sc_0_after_round = paal::data_structures::replace<ls::ObjFunction>([&iter, sc](int & s){
                    return ++iter > static_cast<int>(sc.call<ls::GetMoves>(s).size()) ? -10000 : sc.call<ls::ObjFunction>(s);
                },
                sc
            );

    s = 0;
    LOGLN("first improving 4 checks");
    ls::obj_fun_first_improving(s, sc_0_after_round);
    BOOST_CHECK_EQUAL(s, 4);

    iter = -1;
    s = 0;
    LOGLN("best improving 4 checks");
    ls::obj_fun_best_improving(s, sc_0_after_round);
    BOOST_CHECK_EQUAL(s, 10);

}
