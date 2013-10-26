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


BOOST_AUTO_TEST_CASE(CopyAssymetricArrayMetric) {
    paal::data_structures::AssymetricArrayMetric<int> m(1,2);
    m(0,0) = 1;
    m(0,1) = 2;


    auto r1 = boost::irange(0,1);
    auto r2 = boost::irange(0,2);
    paal::data_structures::AssymetricArrayMetric<int> copy(
            m, boost::begin(r1), boost::end(r1), boost::begin(r2), boost::end(r2));

    BOOST_CHECK(copy(0, 0) == 1);
    BOOST_CHECK(copy(0, 1) == 2);
}

BOOST_AUTO_TEST_CASE(CopyArrayMetric) {
    paal::data_structures::ArrayMetric<int> m(1);
    m(0,0) = 13;

    auto r1 = boost::irange(0,1);
    paal::data_structures::ArrayMetric<int> copy(
            m, boost::begin(r1), boost::end(r1));

    BOOST_CHECK(copy(0, 0) == 13);
}
