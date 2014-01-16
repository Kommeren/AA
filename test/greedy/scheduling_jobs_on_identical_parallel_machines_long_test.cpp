/**
 * @file scheduling_jobs_on_identical_parallel_machines_long_test.cpp
 * @brief 
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2013-09-06
 */

#include <vector>
#include <boost/test/unit_test.hpp>
#include "paal/greedy/scheduling_jobs_on_identical_parallel_machines/scheduling_jobs_on_identical_parallel_machines.hpp"
#include "utils/logger.hpp"
#include "../utils/scheduling.hpp"
const long long maxTime=1000000000;
const long minMachines=10;
const long maxMachines=1000000;
const long stepMachines=10;
const double minJobsOnMachineStart=1.0;
const double minJobsOnMachineEnd=5.0;
const double minJobsOnMachineStep=0.33;
const long seed=42;
typedef long long Time;

BOOST_AUTO_TEST_CASE(test_1) {
    std::srand(seed);
    for(int numberOfMachines=minMachines;
         numberOfMachines<=maxMachines;
         numberOfMachines*=stepMachines){
        for(double minJobsOnMachine=minJobsOnMachineStart;
                minJobsOnMachine<minJobsOnMachineEnd;
                minJobsOnMachine+=minJobsOnMachineStep){
            LOGLN("machines: "<<numberOfMachines);

            std::vector<int> machines(numberOfMachines);
            std::vector<Time> jobs = generateJobLoads(machines,minJobsOnMachine,
               maxTime,paal::utils::ReturnSomethingFunctor<int, 1>());
            LOGLN("jobs: "<<jobs.size());

            std::vector< std::pair<int, decltype(jobs)::iterator> > result;

            paal::greedy::schedulingJobsOnIdenticalParallelMachines(numberOfMachines,
                     jobs.begin(),jobs.end(),back_inserter(result),
                     paal::utils::IdentityFunctor());
            checkJobs(result, jobs);
            std::vector<Time> sumOfMachine;
            sumOfMachine.resize(numberOfMachines);
            for(auto jobMachinePair:result){
                sumOfMachine[jobMachinePair.first]+=*(jobMachinePair.second);
            }

            Time maximumLoad=*std::max_element(sumOfMachine.begin(),sumOfMachine.end());

            Time sumAllLoads=std::accumulate(sumOfMachine.begin(),sumOfMachine.end(),0.0);
            auto upperBoundAproxmiationRatio=
               (maximumLoad*numberOfMachines)/double(sumAllLoads);
            //print result
            LOGLN("Solution: "<<maximumLoad);
            LOGLN("Approximation Ratio: "<<upperBoundAproxmiationRatio);
            LOGLN("");
            BOOST_CHECK(upperBoundAproxmiationRatio<=4.0/3.0);
            BOOST_CHECK(upperBoundAproxmiationRatio>=1);
        }
    }
}
