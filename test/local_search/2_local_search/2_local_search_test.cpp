//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file 2_local_search_test.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-09-20
 */
#include "utils/sample_graph.hpp"
#include "utils/logger.hpp"
#include "2_local_search_logger.hpp"

#include "paal/local_search/2_local_search/2_local_search.hpp"
#include "paal/data_structures/cycle/simple_cycle.hpp"

#include <boost/test/unit_test.hpp>

#include <vector>
#include <string>

using std::string;
using std::vector;
using namespace paal::local_search;
using namespace paal;

BOOST_AUTO_TEST_CASE(two_local_search_test) {
    //! [Two Local Search Example]
    // sample data
    typedef sample_graphs_metrics SGM;
    auto gm = SGM::get_graph_metric_small();
    const int size = gm.size();
    std::vector<int> v(size);
    std::iota(v.begin(), v.end(), 0);

    // create random solution
    std::random_shuffle(v.begin(), v.end());
    typedef data_structures::simple_cycle<int> Cycle;
    Cycle cycle(v.begin(), v.end());

    // creating local search components
    auto lsc = get_default_two_local_components(gm);

    // printing
    LOGLN("Length \t" << get_cycle_length(gm, cycle));

    // setting logger
    auto logger = utils::make_two_ls_logger(gm, 100);

    // search
    two_local_search(cycle, local_search::first_improving_strategy{}, logger,
                     utils::always_false(), lsc);
    //! [Two Local Search Example]
}
