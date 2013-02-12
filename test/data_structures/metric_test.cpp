#define BOOST_TEST_MODULE cycle_manager

#include <boost/test/unit_test.hpp>
#include "utils/sample_graph.hpp"

BOOST_AUTO_TEST_CASE(MetricTest) {
    typedef  SampleGraphsMetrics SGM;
    auto gm = SGM::getGraphMetricSmall();

    BOOST_CHECK(gm(SGM::A,SGM::B) == 2);
    BOOST_CHECK(gm(SGM::C,SGM::B) == 3);
}
