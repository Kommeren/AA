/**
 * @files zelikovsky_11_per_6_long_test.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-04
 */

#include <boost/test/unit_test.hpp>

// this include must be here! //hack for clang
#include "paal/steiner_tree/zelikovsky_11_per_6.hpp"

#include "utils/logger.hpp"
#include "utils/read_steinlib.hpp"
#include "utils/test_result_check.hpp"

#include "paal/data_structures/bimap.hpp"

#include <vector>

BOOST_AUTO_TEST_CASE(zelikovsky_11_per_6_test) {
    std::vector<paal::steiner_tree_test> data;
    LOGLN("READING INPUT...");
    read_steinlib_tests(data);
    for (paal::steiner_tree_test &test : data) {
        LOGLN("TEST " << test.test_name);
        LOGLN("OPT " << test.optimal);

        typedef decltype(test.metric) Metric;
        typedef paal::data_structures::voronoi<Metric> voronoiT;
        typedef typename voronoiT::GeneratorsSet FSet;
        voronoiT voronoi(
            FSet(test.terminals.begin(), test.terminals.end()),
            FSet(test.steiner_points.begin(), test.steiner_points.end()),
            test.metric);
        std::vector<int> selectedSteinerPoints;
        paal::steiner_tree_zelikovsky11per6approximation(
            test.metric, voronoi, std::back_inserter(selectedSteinerPoints));

        auto resRange = boost::join(test.terminals, selectedSteinerPoints);
        LOG_COPY_RANGE_DEL(resRange, ",");
        paal::data_structures::bimap<int> idx;
        auto g = paal::data_structures::metric_to_bgl_with_index(
            test.metric, resRange, idx);
        std::vector<int> pm(resRange.size());
        boost::prim_minimum_spanning_tree(g, &pm[0]);
        auto idxM = paal::data_structures::make_metric_on_idx(test.metric, idx);
        int res(0);
        for (int i : boost::irange(0, int(pm.size()))) {
            if (pm[i] != i) {
                res += idxM(i, pm[i]);
            }
        }
        check_result(res, test.optimal, 11. / 6);
    }

    // BOOST_CHECK_EQUAL(s, 6);
}
