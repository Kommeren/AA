/**
 * @file iterative_rounding_test.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-04
 */

#include <boost/test/unit_test.hpp>

#include <vector>

#include "utils/logger.hpp"
#include "paal/iterative_rounding/iterative_rounding.hpp"
#include "paal/iterative_rounding/generalised_assignment/generalised_assignment.hpp"

using namespace  paal;
using namespace  paal::ir;

struct LogVisitor : public TrivialVisitor {

    template <typename Problem, typename LP>
    void roundCol(const Problem &, LP & lp, lp::ColId col, double val) {
        LOGLN("Column "<< col.get() << " rounded to " << val);
    }

    template <typename Problem, typename LP>
    void relaxRow(const Problem &, LP & lp, lp::RowId row) {
        LOGLN("Relax row " << row.get());
    }
};

BOOST_AUTO_TEST_SUITE(generalised_assignment)
BOOST_AUTO_TEST_CASE(generalised_assignment_test) {
    //sample problem
    LOGLN("sample problem:");
    auto machines = boost::irange(0,2);
    auto jobs = boost::irange(0,2);

    std::vector<std::vector<int>> cost(2, std::vector<int>(2));
    cost[0][0] = 2;
    cost[0][1] = 3;
    cost[1][0] = 1;
    cost[1][1] = 3;
    auto  costf = [&](int i, int j){return cost[i][j];};


    std::vector<std::vector<int>> time(2, std::vector<int>(2));
    time[0][0] = 2;
    time[0][1] = 2;
    time[1][0] = 1;
    time[1][1] = 1;
    auto  timef = [&](int i, int j){return time[i][j];};

    std::vector<int> T = {2, 2};
    auto  Tf = [&](int i){return T[i];};

    std::unordered_map<int, int> jobsToMachines;

    paal::ir::generalised_assignment_iterative_rounding(
        machines.begin(), machines.end(),
        jobs.begin(), jobs.end(),
        costf, timef, Tf, std::inserter(jobsToMachines, jobsToMachines.begin()),
        paal::ir::GeneralAssignmentIRComponents<>(), LogVisitor());

    ON_LOG(for(const std::pair<int, int> & jm : jobsToMachines) {
        LOGLN("Job " << jm.first << " assigned to Machine " << jm.second);
    })

   auto j0 = jobsToMachines.find(0);
   BOOST_CHECK(j0 != jobsToMachines.end() && j0->second == 0);

   auto j1 = jobsToMachines.find(1);
   BOOST_CHECK(j1 != jobsToMachines.end() && j1->second == 0);

   //compile with trivial visitor
   {
        std::unordered_map<int, int> jobsToMachines2;
        paal::ir::generalised_assignment_iterative_rounding(
           machines.begin(), machines.end(),
           jobs.begin(), jobs.end(),
           costf, timef, Tf, std::inserter(jobsToMachines2, jobsToMachines2.begin()),
           paal::ir::GeneralAssignmentIRComponents<>());
   }

}

BOOST_AUTO_TEST_CASE(generalised_assignment_infeasible_test) {
    // infeasible problem
    LOGLN("infeasible problem:");
    auto machines = boost::irange(0,2);
    auto jobs = boost::irange(0,2);

    std::vector<std::vector<int>> cost(2, std::vector<int>(2));
    cost[0][0] = 2;
    cost[0][1] = 3;
    cost[1][0] = 1;
    cost[1][1] = 3;
    auto  costf = [&](int i, int j){return cost[i][j];};

    std::vector<std::vector<int>> time(2, std::vector<int>(2));
    time[0][0] = 2;
    time[0][1] = 4;
    time[1][0] = 3;
    time[1][1] = 3;
    auto  timef = [&](int i, int j){return time[i][j];};

    std::vector<int> T = {2, 2};
    auto  Tf = [&](int i){return T[i];};

    std::vector<std::pair<int, int>> jobsToMachines;

    auto probType = paal::ir::generalised_assignment_iterative_rounding(
        machines.begin(), machines.end(),
        jobs.begin(), jobs.end(),
        costf, timef, Tf, std::back_inserter(jobsToMachines),
        paal::ir::GeneralAssignmentIRComponents<>());

    BOOST_CHECK(probType == lp::INFEASIBLE);
}
BOOST_AUTO_TEST_SUITE_END()

