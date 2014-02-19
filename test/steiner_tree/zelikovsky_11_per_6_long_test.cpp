/**
 * @files zelikovsky_11_per_6_long_test.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-04
 */

#include <boost/test/unit_test.hpp>

#include <vector>

#include "utils/logger.hpp"

#include "paal/steiner_tree/zelikovsky_11_per_6.hpp"
#include "paal/data_structures/bimap.hpp"

#include "utils/read_steinlib.hpp"

BOOST_AUTO_TEST_CASE(zelikovsky_11_per_6_test) {
    std::vector<paal::SteinerTreeTest> data;
    LOGLN("READING INPUT...");
    readSTEINLIBtests(data);
    for (paal::SteinerTreeTest& test : data) {
        LOGLN("TEST " << test.testName);
        LOGLN("OPT " << test.optimal);

        typedef decltype(test.metric) Metric;
        typedef paal::data_structures::Voronoi<Metric> VoronoiT;
        typedef typename VoronoiT::GeneratorsSet FSet;
        VoronoiT voronoi(FSet(test.terminals.begin(), test.terminals.end()),
                         FSet(test.steinerPoints.begin(), test.steinerPoints.end()), test.metric);
        std::vector<int> selectedSteinerPoints;
        paal::steiner_tree::steinerTreeZelikovsky11per6approximation(test.metric, voronoi, std::back_inserter(selectedSteinerPoints));

        auto resRange = boost::join(test.terminals, selectedSteinerPoints);
        LOG_COPY_RANGE_DEL(resRange, ",");
        paal::data_structures::BiMap<int> idx;
        auto g = paal::data_structures::metricToBGLWithIndex(test.metric, boost::begin(resRange), boost::end(resRange), idx);
        std::vector<int> pm(resRange.size());
        boost::prim_minimum_spanning_tree(g, &pm[0]);
        auto idxM = paal::data_structures::make_metricOnIdx(test.metric, idx);
        int res(0);
        for(int i : boost::irange(0, int(pm.size()))) {
            if(pm[i] != i) {
                res += idxM(i, pm[i]);
            }
        }
        LOGLN("RES " << res);

        auto appRatio = double(res) / double(test.optimal);
        BOOST_CHECK(appRatio <= 11./6.);
        LOGLN("APPROXIMATION_RATIO:" << appRatio);
    }

   //BOOST_CHECK_EQUAL(s, 6);
}
