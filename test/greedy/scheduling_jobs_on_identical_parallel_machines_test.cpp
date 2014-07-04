/**
 * @file scheduling_jobs_on_identical_parallel_machines_test.cpp
 * @brief
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2013-09-06
 */

#include "utils/logger.hpp"
#include "utils/scheduling.hpp"
#include "utils/test_result_check.hpp"

#include "paal/greedy/scheduling_jobs_on_identical_parallel_machines/scheduling_jobs_on_identical_parallel_machines.hpp"

#include <boost/test/unit_test.hpp>

#include <vector>

BOOST_AUTO_TEST_CASE(scheduling_jobs_on_identical_parallel_machines) {
    // sample data
    int NUMBER_OF_MACHINES = 3;
    typedef double Time;
    std::vector<Time> j = { 2.1, 3.1, 4.1, 5.1, 6.1, 7.1, 8.1 };
    std::vector<std::pair<int, decltype(j)::iterator>> result;
    paal::greedy::scheduling_jobs_on_identical_parallel_machines(
        NUMBER_OF_MACHINES, j.begin(), j.end(), back_inserter(result),
        paal::utils::identity_functor());
    check_jobs(result, j);
    std::vector<Time> sum_of_machine;
    sum_of_machine.resize(NUMBER_OF_MACHINES);
    for (auto job_machine_pair : result) {
        sum_of_machine[job_machine_pair.first] += *job_machine_pair.second;
    }

    Time maximumLoad =
        *std::max_element(sum_of_machine.begin(), sum_of_machine.end());

    Time sum_all_loads =
        std::accumulate(sum_of_machine.begin(), sum_of_machine.end(), 0.0);
    // print result
    check_result(maximumLoad, double(sum_all_loads) / NUMBER_OF_MACHINES,
                 4.0 / 3.0);
}
