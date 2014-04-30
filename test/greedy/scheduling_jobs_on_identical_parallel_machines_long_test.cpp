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
const long long max_time=1000000000;
const long min_machines=10;
const long max_machines=1000000;
const long step_machines=10;
const double min_jobs_on_machine_start=1.0;
const double min_jobs_on_machine_end=5.0;
const double min_jobs_on_machine_step=0.33;
const long seed=42;
typedef long long Time;

BOOST_AUTO_TEST_CASE(test_1) {
    std::srand(seed);
    for(int numberOfMachines=min_machines;
         numberOfMachines<=max_machines;
         numberOfMachines*=step_machines){
        for(double minJobsOnMachine=min_jobs_on_machine_start;
                minJobsOnMachine<min_jobs_on_machine_end;
                minJobsOnMachine+=min_jobs_on_machine_step){
            LOGLN("machines: "<<numberOfMachines);

            std::vector<int> machines(numberOfMachines);
            std::vector<Time> jobs = generate_job_loads(machines,minJobsOnMachine,
               max_time,paal::utils::return_something_functor<int, 1>());
            LOGLN("jobs: "<<jobs.size());

            std::vector< std::pair<int, decltype(jobs)::iterator> > result;

            paal::greedy::scheduling_jobs_on_identical_parallel_machines(numberOfMachines,
                     jobs.begin(),jobs.end(),back_inserter(result),
                     paal::utils::identity_functor());
            check_jobs(result, jobs);
            std::vector<Time> sumOfMachine;
            sumOfMachine.resize(numberOfMachines);
            for(auto jobMachinePair:result){
                sumOfMachine[jobMachinePair.first]+=*(jobMachinePair.second);
            }

            Time maximumLoad=*std::max_element(sumOfMachine.begin(),sumOfMachine.end());

            Time sumAllLoads=std::accumulate(sumOfMachine.begin(),sumOfMachine.end(),0.0);
            //print result
            check_result(double(maximumLoad),double(sumAllLoads)/numberOfMachines,4.0/3.0);
        }
    }
}
