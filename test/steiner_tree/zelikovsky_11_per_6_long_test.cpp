//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
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

#include "paal/utils/irange.hpp"
#include "paal/data_structures/bimap.hpp"

#include <vector>

BOOST_AUTO_TEST_CASE(zelikovsky_11_per_6_test) {
    std::vector<paal::steiner_tree_test_with_metric> data;
    LOGLN("READING INPUT...");
    read_steinlib_tests(data);
    for (const auto &test : data) {
        LOGLN("TEST " << test.test_name);
        LOGLN("OPT " << test.optimal);

        using Metric = decltype(test.metric);
        using voronoiT = paal::data_structures::voronoi<Metric>;
        using FSet = typename voronoiT::GeneratorsSet;
        voronoiT voronoi(
            FSet(test.terminals.begin(), test.terminals.end()),
            FSet(test.steiner_points.begin(), test.steiner_points.end()),
            test.metric);
        std::vector<int> selected_steiner_points;
        paal::steiner_tree_zelikovsky11per6approximation(
            test.metric, voronoi, std::back_inserter(selected_steiner_points));

        auto res_range = boost::join(test.terminals, selected_steiner_points);
        LOG_COPY_RANGE_DEL(res_range, ",");
        paal::data_structures::bimap<int> idx;
        auto g = paal::data_structures::metric_to_bgl_with_index(
            test.metric, res_range, idx);
        std::vector<int> pm(res_range.size());
        boost::prim_minimum_spanning_tree(g, &pm[0]);
        auto idx_m = paal::data_structures::make_metric_on_idx(test.metric, idx);
        int res(0);
        for (int i : paal::irange(pm.size())) {
            if (pm[i] != i) {
                res += idx_m(i, pm[i]);
            }
        }
        check_result(res, test.optimal, 11. / 6);
    }
}
