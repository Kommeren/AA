/**
 * @file steiner_tree_long_test.cpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-04
 */

#define BOOST_TEST_MODULE steiner_tree_long

#include <boost/test/unit_test.hpp>

#include <vector>
#include <fstream>

#include "utils/logger.hpp"

#include "paal/steiner_tree/zelikovsky_11_per_6.hpp"
#include "paal/data_structures/bimap.hpp"

#include "utils/read_steinlib.hpp"
#include "utils/sample_graph.hpp"

BOOST_AUTO_TEST_CASE(metric_to_bgl_mst_test) {
    std::vector<paal::SteinerTreeTest> data;
    LOG("READING INPUT...");
    readSTEINLIBtests(data);
    for (paal::SteinerTreeTest& test : data) {
        LOG("TEST " << test.testName);
        LOG("OPT " << test.optimal);

        typedef decltype(test.metric) Metric;
        typedef paal::data_structures::Voronoi<Metric> VoronoiT;
        typedef typename VoronoiT::GeneratorsSet FSet;
        VoronoiT voronoi(FSet(test.terminals.begin(), test.terminals.end()),
                         FSet(test.steinerPoints.begin(), test.steinerPoints.end()), test.metric);
        std::vector<int> selectedSteinerPoints = paal::steiner_tree::getSteinerVertices(test.metric, voronoi);

        auto resRange = boost::join(test.terminals, selectedSteinerPoints);
        LOG_COPY_DEL(boost::begin(resRange), boost::end(resRange), ",");
        paal::data_structures::BiMap<int> idx;
        auto g = paal::data_structures::metricToBGLWithIndex(test.metric, boost::begin(resRange), boost::end(resRange), idx);
        std::vector<int> pm(resRange.size());
        boost::prim_minimum_spanning_tree(g, &pm[0]);
        auto idxM = paal::data_structures::make_metricOnIdx(test.metric, idx);
        int res(0);
        for(int i : boost::irange(0, int(pm.size()))) {
            res += idxM(i, pm[i]);
        }
        LOG("RES " << res);

        LOG("APPROXIMATION_RATIO:" << double(res) / double(test.optimal));
    }

   //BOOST_CHECK_EQUAL(s, 6);
}
