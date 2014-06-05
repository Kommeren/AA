/**
 * @file iterative_rounding_test.cpp
 * @brief
 * @author Piotr Wygocki, Piotr Godlewski
 * @version 1.0
 * @date 2013-02-04
 */

#include "utils/logger.hpp"
#include "iterative_rounding/log_visitor.hpp"

#include "paal/data_structures/metric/basic_metrics.hpp"
#include "paal/iterative_rounding/generalised_assignment/generalised_assignment.hpp"

#include <boost/test/unit_test.hpp>

#include <vector>

using namespace paal;
using namespace paal::ir;


BOOST_AUTO_TEST_SUITE(generalised_assignment)
BOOST_AUTO_TEST_CASE(generalised_assignment_test) {
     //sample problem
     LOGLN("sample problem:");
     auto machines = boost::irange(0,2);
     auto jobs = boost::irange(0,2);

     paal::data_structures::array_metric<int> cost{2};
     cost(0, 0) = 2;
     cost(0, 1) = 3;
     cost(1, 0) = 1;
     cost(1, 1) = 3;

     paal::data_structures::array_metric<int> time{2};
     time(0, 0) = 2;
     time(0, 1) = 2;
     time(1, 0) = 1;
     time(1, 1) = 1;

     auto T = [&](int i){return 2;};

     std::unordered_map<int, int> jobs_to_machines;

     paal::ir::generalised_assignment_iterative_rounding(
         machines.begin(), machines.end(),
         jobs.begin(), jobs.end(),
         cost, time, T, std::inserter(jobs_to_machines, jobs_to_machines.begin()),
         paal::ir::ga_ir_components<>{}, log_visitor{});

     ON_LOG(for(const std::pair<int, int> & jm : jobs_to_machines) {
         LOGLN("Job " << jm.first << " assigned to Machine " << jm.second);
     })

    auto j0 = jobs_to_machines.find(0);
    BOOST_CHECK(j0 != jobs_to_machines.end() && j0->second == 0);

    auto j1 = jobs_to_machines.find(1);
    BOOST_CHECK(j1 != jobs_to_machines.end() && j1->second == 0);

    //compile with trivial visitor
    {
         jobs_to_machines.clear();
         paal::ir::generalised_assignment_iterative_rounding(
            machines.begin(), machines.end(),
            jobs.begin(), jobs.end(),
            cost, time, T, std::inserter(jobs_to_machines, jobs_to_machines.begin()));
    }
}

BOOST_AUTO_TEST_CASE(generalised_assignment_infeasible_test) {
    // infeasible problem
    LOGLN("infeasible problem:");
    auto machines = boost::irange(0,2);
    auto jobs = boost::irange(0,2);

     paal::data_structures::array_metric<int> cost{2};
     cost(0, 0) = 2;
     cost(0, 1) = 3;
     cost(1, 0) = 1;
     cost(1, 1) = 3;

     paal::data_structures::array_metric<int> time{2};
     time(0, 0) = 2;
     time(0, 1) = 4;
     time(1, 0) = 3;
     time(1, 1) = 3;

    auto  T = [&](int i){return 2;};

    std::vector<std::pair<int, int>> jobs_to_machines;

    auto result = paal::ir::generalised_assignment_iterative_rounding(
        machines.begin(), machines.end(),
        jobs.begin(), jobs.end(),
        cost, time, T, std::back_inserter(jobs_to_machines));

    BOOST_CHECK(result.first == lp::INFEASIBLE);
    BOOST_CHECK(!result.second);
}
BOOST_AUTO_TEST_SUITE_END()

