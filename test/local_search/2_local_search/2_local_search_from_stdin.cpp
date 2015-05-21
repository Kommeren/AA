//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file 2_local_search_from_stdin.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-09-20
 */
#define BOOST_TEST_MODULE two_local_search_stdin

#include "test_utils/read_tsplib.hpp"
#include "test_utils/2_local_search_logger.hpp"

#include "paal/local_search/2_local_search/2_local_search.hpp"
#include "paal/data_structures/cycle/simple_cycle.hpp"
#include "paal/data_structures/cycle/splay_cycle.hpp"
#include "paal/local_search/custom_components.hpp"
#include "paal/data_structures/components/components_replace.hpp"

#include <boost/test/unit_test.hpp>

#include <vector>
#include <string>

using std::vector;
using namespace paal::local_search;
using namespace paal;

template <typename Cycle> void test() {
    read_tsplib::TSPLIB_Directory::Graph graph(std::cin);
    read_tsplib::TSPLIB_Matrix mtx;

    graph.load(mtx);
    auto size = mtx.size();
    std::vector<int> v(size);
    std::iota(v.begin(), v.end(), 0);

    // create random solution
    std::random_shuffle(v.begin(), v.end());
    LOG_COPY_RANGE_DEL(v, ",");
    Cycle cycle(v.begin(), v.end());
    LOG_COPY_DEL(cycle.vbegin(), cycle.vend(), ",");

    // printing
    LOG_COPY_DEL(cycle.vbegin(), cycle.vend(), ",");
    LOGLN("Length before\t" << get_cycle_length(mtx, cycle));

    // setting logger
    auto logger = utils::make_two_ls_logger(mtx);

    // search
    two_local_search(cycle, paal::local_search::first_improving_strategy{},
                     logger, utils::always_false(),
                     get_default_two_local_components(mtx));
}

BOOST_AUTO_TEST_CASE(TSPLIB_simple) {
    test<paal::data_structures::simple_cycle<int>>();
}

BOOST_AUTO_TEST_CASE(TSPLIB) {
    test<paal::data_structures::splay_cycle<int>>();
}

using paal::local_search::search_components;

// TODO probably optimization does not optimize...
BOOST_AUTO_TEST_CASE(TSPLIB_cut) {
    read_tsplib::TSPLIB_Directory::Graph graph(std::cin);
    read_tsplib::TSPLIB_Matrix mtx;
    graph.load(mtx);
    auto size = mtx.size();
    std::vector<int> v(size);
    std::iota(v.begin(), v.end(), 0);

    // create random solution
    typedef data_structures::simple_cycle<int> Cycle;
    std::random_shuffle(v.begin(), v.end());
    Cycle cycle(v.begin(), v.end());
    int startLen = get_cycle_length(mtx, cycle);

    // creating local search
    auto lsc = get_default_two_local_components(mtx);
    typedef local_search::search_components_traits<puretype(lsc)>::GainT GainT;
    typedef local_search::gain_cut_small_improves<GainT, int> CIC;
    double epsilon = 0.001;
    CIC cut(lsc.get<paal::local_search::Gain>(), startLen, epsilon);
    auto cutLsc =
        data_structures::replace<local_search::Gain>(std::move(cut), lsc);

    // setting logger
    auto logger = utils::make_two_ls_logger(mtx);

    // printing
    LOGLN("Length before\t" << get_cycle_length(mtx, cycle));
    paal::local_search::first_improving_strategy strategy{};

    // search
    for (int j = 0; j < 20; ++j) {
        epsilon /= 2;
        LOGLN("epsilon = " << epsilon);
        cutLsc.get<local_search::Gain>().set_epsilon(epsilon);
        two_local_search(cycle, strategy, logger, utils::always_false(),
                         cutLsc);
    }

    LOGLN("Normal search at the end");
    two_local_search(cycle, strategy, logger, utils::always_false(), lsc);
}
