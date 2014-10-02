//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file generalised_assignment_long_test.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-15
 */

#include "utils/logger.hpp"
#include "utils/parse_file.hpp"
#include "utils/read_gen_ass.hpp"

#include "paal/data_structures/components/components_replace.hpp"
#include "paal/data_structures/metric/basic_metrics.hpp"
#include "paal/iterative_rounding/generalised_assignment/generalised_assignment.hpp"

#include <boost/test/unit_test.hpp>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>

using namespace paal::ir;
using namespace paal;

template <typename Machines, typename JobsToMachines, typename Times,
        typename MachineBounds, typename Jobs>
void check_result(IRResult result, const Machines & machines,
        const JobsToMachines & jobs_to_machines, const Times & times,
        const MachineBounds & machines_bounds, const Jobs & jobs, int opt) {

    BOOST_CHECK(result.first == lp::OPTIMAL);

    std::vector<int> machines_load(machines.size(), 0);
    for (auto jm : jobs_to_machines) {
        LOGLN("job " << jm.first << " assigned to machine " << jm.second);
        machines_load[jm.second] += times(jm.first, jm.second);
    }
    double approximation_ratio = 1.;
    for (int m : machines) {
        BOOST_CHECK(machines_load[m] <= 2 * machines_bounds[m]);
        approximation_ratio = std::max(approximation_ratio, double(machines_load[m]) / double(machines_bounds[m]));
    }

    for (int j : jobs) {
        BOOST_CHECK(jobs_to_machines.find(j) != jobs_to_machines.end());
    }
    BOOST_CHECK(abs(std::round(*(result.second))-*(result.second)<0.000001));
    int c = std::round(*(result.second));
    LOGLN("cost " << c);
    BOOST_CHECK(c <= opt);
    LOGLN(std::setprecision(10) << "APPROXIMATION RATIO: " << approximation_ratio << " cost / opt = " << double(c) / double(opt));
}

BOOST_AUTO_TEST_CASE(generalised_assignment_long) {
    std::string test_dir = "test/data/GENERALISED_ASSIGNMENT/";
    parse(test_dir + "gapopt.txt", [&](const std::string & fname, std::istream & is_test_cases) {
        int opt;
        int number_of_cases;

        is_test_cases >> number_of_cases;

        LOGLN(fname << " " << number_of_cases);
        std::ifstream ifs(test_dir + "/cases/" + fname + ".txt");
        int num;
        ifs >> num;
        assert(num == number_of_cases);
        for(int i = 0; i < number_of_cases; ++i) {
            is_test_cases >> opt;
            LOGLN("case " << i << " opt " << opt);

            paal::M costs;
            paal::M times;
            std::vector<int> machines_bounds;
            boost::integer_range<int> machines(0,0);
            boost::integer_range<int> jobs(0,0);
            paal::read_gen_ass(ifs, costs, times, machines_bounds, machines, jobs);
            auto Tf = [&](int i){return machines_bounds[i];};
            {
                LOGLN("Unlimited relaxations");
                std::unordered_map<int, int> jobs_to_machines;
                auto result = generalised_assignment_iterative_rounding(
                    machines.begin(), machines.end(),
                    jobs.begin(), jobs.end(),
                    costs, times, Tf, std::inserter(jobs_to_machines, jobs_to_machines.begin()));
                check_result(result, machines, jobs_to_machines, times, machines_bounds, jobs, opt);
            }
            {
                LOGLN("Relaxations limit = 1/iter");
                std::unordered_map<int, int> jobs_to_machines;
                ir::ga_ir_components<> comps;
                auto components =
                    paal::data_structures::replace<ir::RelaxationsLimit>(
                        ir::relaxations_limit_condition(), comps);
                auto result = generalised_assignment_iterative_rounding(
                    machines.begin(), machines.end(),
                    jobs.begin(), jobs.end(),
                    costs, times, Tf, std::inserter(jobs_to_machines, jobs_to_machines.begin()),
                    components);
                check_result(result, machines, jobs_to_machines, times, machines_bounds, jobs, opt);
            }
        }
        int MAX_LINE = 256;
        char buf[MAX_LINE];
        is_test_cases.getline(buf, MAX_LINE);
    });
}
