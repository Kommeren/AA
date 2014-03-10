/**
 * @file generalised_assignment_long_test.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-15
 */

#include "utils/logger.hpp"
#include "utils/read_gen_ass.hpp"
#include "utils/parse_file.hpp"

#include "paal/data_structures/metric/basic_metrics.hpp"
#include "paal/data_structures/components/components_replace.hpp"
#include "paal/iterative_rounding/generalised_assignment/generalised_assignment.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/range/irange.hpp>

#include <iterator>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace paal::ir;
using namespace paal;

template <typename Machines, typename JobsToMachines, typename Times,
        typename MachineBounds, typename Jobs>
void check_result(IRResult result, const Machines & machines,
        const JobsToMachines & jobsToMachines, const Times & times,
        const MachineBounds & machinesBounds, const Jobs & jobs, int opt) {

    BOOST_CHECK(result.first == lp::OPTIMAL);

    std::vector<int> machinesLoad(machines.size(), 0);
    for (const std::pair<int, int> & jm : jobsToMachines) {
        LOGLN("job " << jm.first << " assigned to machine " << jm.second);
        machinesLoad[jm.second] += times(jm.first, jm.second);
    }
    double approximationRatio = 1.;
    for (int m : machines) {
        BOOST_CHECK(machinesLoad[m] <= 2 * machinesBounds[m]);
        approximationRatio = std::max(approximationRatio, double(machinesLoad[m]) / double(machinesBounds[m]));
    }

    for (int j : jobs) {
        BOOST_CHECK(jobsToMachines.find(j) != jobsToMachines.end());
    }
    BOOST_CHECK(abs(std::round(*(result.second))-*(result.second)<0.000001));
    int c=std::round(*(result.second));
    LOGLN("cost " << c);
    BOOST_CHECK(c <= opt);
    LOGLN(std::setprecision(10) << "APPROXIMATION RATIO: " << approximationRatio << " cost / opt = " << double(c) / double(opt));
}

BOOST_AUTO_TEST_CASE(generalised_assignmentLong) {
    std::string testDir = "test/data/GENERALISED_ASSIGNMENT/";
    parse(testDir + "gapopt.txt", [&](const std::string & fname, std::istream & is_test_cases) {
        int opt;
        int numberOfCases;

        is_test_cases >> numberOfCases;

        LOGLN(fname << " " << numberOfCases);
        std::ifstream ifs(testDir + "/cases/" + fname + ".txt");
        int num;
        ifs >> num;
        assert(num == numberOfCases);
        for(int i = 0; i < numberOfCases; ++i) {
            is_test_cases >> opt;
            LOGLN("case " << i << " opt "<< opt);

            paal::M costs;
            paal::M times;
            std::vector<int> machinesBounds;
            boost::integer_range<int> machines(0,0);
            boost::integer_range<int> jobs(0,0);
            paal::read_gen_ASS(ifs, costs, times, machinesBounds, machines, jobs);
            auto Tf = [&](int i){return machinesBounds[i];};
            {
                LOGLN("Unlimited relaxations");
                std::unordered_map<int, int> jobsToMachines;
                auto result = generalised_assignment_iterative_rounding(
                    machines.begin(), machines.end(),
                    jobs.begin(), jobs.end(),
                    costs, times, Tf, std::inserter(jobsToMachines, jobsToMachines.begin()));
                check_result(result, machines, jobsToMachines, times, machinesBounds, jobs, opt);
            }
            {
                LOGLN("Relaxations limit = 1/iter");
                std::unordered_map<int, int> jobsToMachines;
                ir::ga_ir_components<> comps;
                auto components = paal::data_structures::replace<ir::RelaxationsLimit>(
                                    ir::relaxations_limit_condition(), comps);
                auto result = generalised_assignment_iterative_rounding(
                    machines.begin(), machines.end(),
                    jobs.begin(), jobs.end(),
                    costs, times, Tf, std::inserter(jobsToMachines, jobsToMachines.begin()),
                    components);
                check_result(result, machines, jobsToMachines, times, machinesBounds, jobs, opt);
            }
        }
        int MAX_LINE = 256;
        char buf[MAX_LINE];
        is_test_cases.getline(buf, MAX_LINE);
    });
}
