/**
 * @file scheduling_jobs_on_identical_parallel_machines_example.cpp
 * @brief 
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2013-09-06
 */
#include <iostream>
#include <utility>
#include "paal/greedy/scheduling_jobs_on_identical_parallel_machines/scheduling_jobs_on_identical_parallel_machines.hpp"
//! [Scheduling Jobs Example]
using namespace paal::greedy::scheduling_jobs_on_identical_parallel_machines;
using namespace std;


/**
 * \brief scheduling jobs on identical parallel machines example
 */
int main() {
    typedef double Time;
    typedef pair<Time,char> Job;

    struct ReturnJobTimeFunctor {
        auto  operator()(Job job) const -> Time
        { 
            return job.first;
        }
    };
    // sample data
    int numberOfMachines = 3;
    vector<Job> jobs={{2.1,'a'},{3.1,'b'},{4.1,'c'},{5.1,'d'},{6.1,'e'},{7.1,'f'},{8.1,'g'}};
    vector<pair<int,Job> > result;
    
    schedulingJobsOnIdenticalParallelMachines(numberOfMachines,jobs.begin(),jobs.end(),back_inserter(result),ReturnJobTimeFunctor());
    
    vector<Time> sumOfMachine;
    sumOfMachine.resize(numberOfMachines);
    for(auto machineJobPair:result){
        auto machine=machineJobPair.first;
        auto job=machineJobPair.second;
        sumOfMachine[machine]+=job.first;
        cout<<"On machine: "<<machine<<" do job: "<<job.second<<endl;
    }
    Time maximumLoad=*std::max_element(sumOfMachine.begin(),sumOfMachine.end());
    
    //print result
    cout<<"Solution:" << maximumLoad << endl;
    return 0;
}
//! [Scheduling Jobs Example]