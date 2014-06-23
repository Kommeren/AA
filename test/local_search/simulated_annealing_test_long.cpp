/**
 * @file simulated_annealing_test_long.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2014-02-04
 */

#include "utils/simple_single_local_search_components.hpp"
#include "utils/logger.hpp"

#include "paal/local_search/simulated_annealing.hpp"
#include "paal/local_search/custom_components.hpp"
#include "paal/data_structures/components/components_replace.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/range/irange.hpp>

#include <thread>

BOOST_AUTO_TEST_SUITE(simulated_annealing)

namespace ls = paal::local_search;
using namespace paal;

BOOST_AUTO_TEST_CASE(CoolingSchemaTest) {
    std::chrono::seconds sec(1);
    auto exponential = ls::make_exponential_cooling_schema_dependant_on_time(
        std::chrono::seconds(10), 1024, 1);
    for (int i : boost::irange(0, 10)) {
        std::this_thread::sleep_for(sec);
        LOGLN("Temperature after " << i << " seconds: " << exponential());
        BOOST_CHECK_EQUAL(exponential(), pow(2, 9 - i));
    }
}

BOOST_AUTO_TEST_SUITE_END()
