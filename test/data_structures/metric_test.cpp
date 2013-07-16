/**
 * @file metric_test.cpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-04
 */
#define BOOST_TEST_MODULE metric

#include <boost/test/unit_test.hpp>
#include "utils/sample_graph.hpp"

BOOST_AUTO_TEST_CASE(MetricTest) {
    typedef  SampleGraphsMetrics SGM;
    auto gm = SGM::getGraphMetricSmall();

    BOOST_CHECK(gm(SGM::A,SGM::B) == 2);
    BOOST_CHECK(gm(SGM::C,SGM::B) == 3);
}
