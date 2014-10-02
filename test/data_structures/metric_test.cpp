//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file metric_test.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-04
 */

#include "utils/sample_graph.hpp"

#include "paal/utils/irange.hpp"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(MetricTest) {
    typedef sample_graphs_metrics SGM;
    auto gm = SGM::get_graph_metric_small();

    BOOST_CHECK(gm(SGM::A, SGM::B) == 2);
    BOOST_CHECK(gm(SGM::C, SGM::B) == 3);
}

BOOST_AUTO_TEST_CASE(Copyrectangle_array_metric) {
    paal::data_structures::rectangle_array_metric<int> m(1, 2);
    m(0, 0) = 1;
    m(0, 1) = 2;

    auto r1 = paal::irange(1);
    auto r2 = paal::irange(2);
    paal::data_structures::rectangle_array_metric<int> copy(
        m, r1, r2);

    BOOST_CHECK(copy(0, 0) == 1);
    BOOST_CHECK(copy(0, 1) == 2);
}

BOOST_AUTO_TEST_CASE(Copyarray_metric) {
    paal::data_structures::array_metric<int> m(1);
    m(0, 0) = 13;

    auto r1 = paal::irange(1);
    paal::data_structures::array_metric<int> copy(m, r1);

    BOOST_CHECK(copy(0, 0) == 13);
}
