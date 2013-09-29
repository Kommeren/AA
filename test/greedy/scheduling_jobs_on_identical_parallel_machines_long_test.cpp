/**
 * @file scheduling_jobs_on_identical_parallel_machines_long_test.cpp
 * @brief 
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2013-09-06
 */
#define BOOST_TEST_MODULE scheduling_jobs_on_identical_parallel_machines_long

#include <vector>
#include <boost/test/unit_test.hpp>
#include "paal/greedy/scheduling_jobs_on_identical_parallel_machines/scheduling_jobs_on_identical_parallel_machines.hpp"
#include "utils/logger.hpp"
const long long maxTime=1000000000;
const long minMachines=10;
const long maxMachines=1000000;
const long stepMachines=10;
const double minJobsOnMachineStart=1.0;
const double minJobsOnMachineEnd=5.0;
const double minJobsOnMachineStep=0.33;
const long seed=42;
typedef long long Time;

std::vector<Time> ingen(int machinesLeft,double minJobsOnMachine){
    std::vector<Time> jobs;
    while(machinesLeft--){
        Time time=maxTime;
        while(time>0){
            Time jobTime=rand()%int(maxTime/minJobsOnMachine);
            jobTime=std::min(jobTime,time);
            jobs.push_back(jobTime);
            time-=jobTime;
        }
    }
    return std::move(jobs);
}

BOOST_AUTO_TEST_CASE(test_1) {
    std::srand(seed);
    for(int numberOfMachines=minMachines;numberOfMachines<=maxMachines;numberOfMachines*=stepMachines){
        for(double minJobsOnMachine=minJobsOnMachineStart;
                minJobsOnMachine<minJobsOnMachineEnd;
                minJobsOnMachine+=minJobsOnMachineStep){
            LOG("machines: "<<numberOfMachines);
            
            std::vector< std::pair<int, Time> > result;
            
            std::vector<Time> jobs=ingen(numberOfMachines,minJobsOnMachine);
            
            LOG("jobs: "<<jobs.size());
            paal::greedy::scheduling_jobs_on_identical_parallel_machines::
                    schedulingJobsOnIdenticalParallelMachines(numberOfMachines,jobs.begin(),jobs.end(),back_inserter(result),paal::utils::IdentityFunctor());
            std::vector<Time> sumOfMachine;
            sumOfMachine.resize(numberOfMachines);
            for(auto jobMachinePair:result){
                sumOfMachine[jobMachinePair.first]+=jobMachinePair.second;
            }
            
            Time maximumLoad=*std::max_element(sumOfMachine.begin(),sumOfMachine.end());
            
            Time sumAllLoads=std::accumulate(sumOfMachine.begin(),sumOfMachine.end(),0.0);
            auto upperBoundAproxmiationRatio=(maximumLoad*numberOfMachines)/double(sumAllLoads);
            //print result
            LOG("Solution: "<<maximumLoad);
            LOG("Aproxmimation Ratio: "<<upperBoundAproxmiationRatio);
            LOG("");
            BOOST_CHECK(upperBoundAproxmiationRatio<=4.0/3.0);
            BOOST_CHECK(upperBoundAproxmiationRatio>=1);
        }
    }
}
