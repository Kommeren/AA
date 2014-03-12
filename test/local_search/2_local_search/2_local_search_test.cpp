/**
 * @file 2_local_search_test.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-09-20
 */
#include <boost/test/unit_test.hpp>

#include <vector>
#include <string>

#include "paal/local_search/2_local_search/2_local_search.hpp"
#include "paal/data_structures/cycle/simple_cycle.hpp"
#include "utils/sample_graph.hpp"
#include "utils/logger.hpp"

#include "2_local_search_logger.hpp"

using std::string;
using std::vector;
using namespace  paal::local_search::two_local_search;
using namespace  paal;


BOOST_AUTO_TEST_CASE(two_local_search_test) {
    //! [Two Local Search Example]
    //sample data
    typedef  SampleGraphsMetrics SGM;
    auto gm = SGM::getGraphMetricSmall();
    const int size = gm.size();
    std::vector<int> v(size);
    std::iota(v.begin(), v.end(), 0);

    //create random solution
    std::random_shuffle(v.begin(), v.end());
    typedef data_structures::SimpleCycle<int> Cycle;
    Cycle cycle(v.begin(), v.end());

    //creating local search components
    auto lsc = getDefaultTwoLocalComponents(gm);

    //printing
    LOGLN("Length \t" << simple_algo::getLength(gm, cycle));

    //setting logger
    auto logger = utils::make_twoLSLogger(gm, 100);

    //search
    two_local_search(cycle, local_search::ChooseFirstBetterStrategy{},
                logger, utils::ReturnFalseFunctor(), lsc);
    //! [Two Local Search Example]
}

