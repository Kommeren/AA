//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file scheduling_jobs_long_test.cpp
 * @brief
 * @author Robert Rosolek
 * @version 1.0
 * @date 2013-11-19
 */
#include "test_utils/scheduling.hpp"
#include "test_utils/logger.hpp"
#include "test_utils/test_result_check.hpp"

#include "paal/utils/irange.hpp"
#include "paal/greedy/scheduling_jobs/scheduling_jobs.hpp"

#include <boost/test/unit_test.hpp>

#include <vector>
#include <utility>

BOOST_AUTO_TEST_CASE(testSchedulingJobs) {
    typedef long long Time;
    typedef std::pair<int, long long> Machine;
    typedef Time Job;

    const unsigned seed = 68;
    const Time optTime = 10000;
    const long min_machines = 10;
    const long max_machines = 100000;
    const long step_machines = 10;
    const long maxMachineSpeed = 100;
    const double min_jobs_on_machine_start = 1.0;
    const double min_jobs_on_machine_end = 5.0;
    const double min_jobs_on_machine_step = 0.33;

    std::srand(seed);
    for (int numberOfMachines = min_machines; numberOfMachines <= max_machines;
         numberOfMachines *= step_machines) {
        for (double minJobsOnMachine = min_jobs_on_machine_start;
             minJobsOnMachine < min_jobs_on_machine_end;
             minJobsOnMachine += min_jobs_on_machine_step) {
            LOGLN("machines: " << numberOfMachines);
            std::vector<Machine> machines(numberOfMachines);
            for (auto machineID : paal::irange(numberOfMachines)) {
                machines[machineID] =
                    std::make_pair(machineID, rand() % maxMachineSpeed + 1);
            }
            auto getSpeed = [](Machine machine) { return machine.second; };

            std::vector<Job> jobs = generate_job_loads(
                machines, minJobsOnMachine, optTime, getSpeed);
            LOGLN("jobs: " << jobs.size());

            typedef std::vector<std::pair<decltype(machines) ::iterator,
                                          decltype(jobs) ::iterator>> Result;
            Result resultRandomized, resultDeterministic;

            paal::greedy::schedule_randomized(
                machines.begin(), machines.end(), jobs.begin(), jobs.end(),
                back_inserter(resultRandomized), getSpeed,
                paal::utils::identity_functor());
            paal::greedy::schedule_deterministic(
                machines.begin(), machines.end(), jobs.begin(), jobs.end(),
                back_inserter(resultDeterministic), getSpeed,
                paal::utils::identity_functor());

            auto checkAndPrint = [&](const Result & result) {
                check_jobs(result, jobs);
                double max_time = get_max_time(result, getSpeed);
                check_result(max_time, double(optTime), 2);
            };

            checkAndPrint(resultRandomized);
            checkAndPrint(resultDeterministic);
        }
    }
}
