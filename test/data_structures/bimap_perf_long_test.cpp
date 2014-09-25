//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file bimap_perf_test.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-04
 */
#include "utils/logger.hpp"

#include "paal/data_structures/bimap.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/range/irange.hpp>

#include <vector>
#include <algorithm>
#include <numeric>

static const int s = 10000000;
static const int nr = 10000000;
std::vector<int> vec(s);
std::vector<int> ids(nr);

template <typename Bimap> void test_get_val(const Bimap &b) {
    int t(0);
    for (int j : boost::irange(0, 50)) {
        std::ignore = j; // removes warning
        for (int i : ids) {
            t += b.get_val(i); // has to do addition, because without it
                               // compiler removes the whole loop
        }
    }
    LOGLN(t);
}

template <typename Bimap> void test_get_idx(const Bimap &b) {
    for (int i : ids) {
        b.get_idx(vec[i]);
    }
}

template <typename Bimap> void test() {
    ON_LOG(time_t t1 = time(NULL));
    Bimap b(vec);
    ON_LOG(time_t t2 = time(NULL));
    LOGLN("construction time " << t2 - t1);
    ON_LOG(t1 = t2);
    test_get_val(b);
    ON_LOG(t2 = time(NULL));
    LOGLN("getval time " << t2 - t1);

    ON_LOG(t1 = t2);
    test_get_idx(b);
    ON_LOG(t2 = time(NULL));
    LOGLN("getidx time " << t2 - t1);
}

BOOST_AUTO_TEST_CASE(bimap) {

    std::iota(vec.begin(), vec.end(), 0);
    std::random_shuffle(vec.begin(), vec.end());

    for (int &i : ids) {
        i = rand() % s;
    }

    LOGLN("boost::mulit_index Bimap");
    test<paal::data_structures::bimap_mic<int>>();
    LOGLN("paal Bimap");
    test<paal::data_structures::bimap<int>>();
}
