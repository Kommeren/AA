/**
 * @file fast_exp.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-06-20
 */

#include <boost/test/unit_test.hpp>

#include <boost/range/irange.hpp>

#include "paal/utils/fast_exp.hpp"
#include "utils/logger.hpp"

BOOST_AUTO_TEST_CASE(FastExpTest) {
    double biggestErr = 0;
    for(int i : boost::irange(-80, 80)) {
        biggestErr = std::max( std::abs(exp(i) - paal::fast_exp(i)) / exp(i), biggestErr);
    }
    LOGLN("biggest error " << biggestErr);
    BOOST_CHECK(biggestErr < 0.04);
}


