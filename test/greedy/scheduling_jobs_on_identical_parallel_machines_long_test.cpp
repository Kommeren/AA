/**
 * @file scheduling_jobs_on_identical_parallel_machines_long_test.cpp
 * @brief
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2013-09-06
 */

#include "utils/logger.hpp"
#include "../utils/scheduling.hpp"
#include "utils/test_result_check.hpp"

#include "paal/greedy/scheduling_jobs_on_identical_parallel_machines/scheduling_jobs_on_identical_parallel_machines.hpp"

#include <boost/test/unit_test.hpp>

#include <vector>
const long long MAX_TIME = 1000000000;
const long MIN_MACHINES = 10;
const long MAX_MACHINES = 1000000;
const long STEP_MACHINES = 10;
const double MIN_JOBS_ON_MACHINE_START = 1.0;
const double MIN_JOBS_ON_MACHINE_END = 5.0;
const double MIN_JOBS_ON_MACHINE_STEP = 0.33;
const long SEED = 42;
using Time = long long;

BOOST_AUTO_TEST_CASE(scheduling_jobs_on_identical_parallel_machines_long) {
    std::srand(SEED);
    for (int number_of_machines = MIN_MACHINES;
         number_of_machines <= MAX_MACHINES;
         number_of_machines *= STEP_MACHINES) {
        for (double min_jobs_on_machine = MIN_JOBS_ON_MACHINE_START;
             min_jobs_on_machine < MIN_JOBS_ON_MACHINE_END;
             min_jobs_on_machine += MIN_JOBS_ON_MACHINE_STEP) {
            LOGLN("machines: " << number_of_machines);

            std::vector<int> machines(number_of_machines);
            std::vector<Time> jobs = generate_job_loads(
                machines, min_jobs_on_machine, MAX_TIME,
                paal::utils::return_something_functor<int, 1>());
            LOGLN("jobs: " << jobs.size());

            std::vector<std::pair<int, decltype(jobs)::iterator>> result;

            paal::greedy::scheduling_jobs_on_identical_parallel_machines(
                number_of_machines, jobs.begin(), jobs.end(),
                back_inserter(result), paal::utils::identity_functor());
            check_jobs(result, jobs);
            std::vector<Time> sum_of_machine;
            sum_of_machine.resize(number_of_machines);
            for (auto job_machine_pair : result) {
                sum_of_machine[job_machine_pair.first] +=
                    *(job_machine_pair.second);
            }

            Time maximum_load =
                *std::max_element(sum_of_machine.begin(), sum_of_machine.end());

            Time sum_all_loads = std::accumulate(sum_of_machine.begin(),
                                                 sum_of_machine.end(), 0.0);
            // print result
            check_result(double(maximum_load),
                         double(sum_all_loads) / number_of_machines, 4.0 / 3.0);
        }
    }
}
