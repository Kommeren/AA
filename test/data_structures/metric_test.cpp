//=======================================================================
// Copyright (c) 2013 Piotr Wygocki
//               2014 Piotr Smulewicz
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file metric_test.cpp
 * @brief
 * @author Piotr Wygocki ,Piotr Smulewicz
 * @version 1.0
 * @date 2013-02-04
 */

#include "test_utils/sample_graph.hpp"

#include "paal/utils/irange.hpp"
#include "paal/utils/floating.hpp"
#include "paal/data_structures/metric/euclidean_metric.hpp"
#include "paal/data_structures/metric/graph_metrics.hpp"
#include "paal/data_structures/metric/basic_metrics.hpp"

#include <boost/test/unit_test.hpp>

#include <cmath>

BOOST_AUTO_TEST_CASE(euclidean_metric_test) {
    auto metric=paal::data_structures::euclidean_metric<int>();
    std::pair<int,int> a={0,0},b={1,1};

    BOOST_CHECK(paal::utils::compare<double>().e(metric(a,b),std::sqrt(2)));
}

BOOST_AUTO_TEST_CASE(metric_test) {
    typedef sample_graphs_metrics SGM;
    auto gm = SGM::get_graph_metric_small();

    BOOST_CHECK_EQUAL(gm(SGM::A, SGM::B) , 2);
    BOOST_CHECK_EQUAL(gm(SGM::C, SGM::B) , 3);
}

BOOST_AUTO_TEST_CASE(copyrectangle_array_metric) {
    paal::data_structures::rectangle_array_metric<int> m(1, 2);
    m(0, 0) = 1;
    m(0, 1) = 2;

    auto r1 = paal::irange(1);
    auto r2 = paal::irange(2);
    paal::data_structures::rectangle_array_metric<int> copy(
        m, r1, r2);

    BOOST_CHECK_EQUAL(copy(0, 0) , 1);
    BOOST_CHECK_EQUAL(copy(0, 1) , 2);
}

BOOST_AUTO_TEST_CASE(copyarray_metric) {
    paal::data_structures::array_metric<int> m(1);
    m(0, 0) = 13;

    auto r1 = paal::irange(1);
    paal::data_structures::array_metric<int> copy(m, r1);

    BOOST_CHECK_EQUAL(copy(0, 0) , 13);
}
