/**
 * @file scheduling_jobs_with_deadlines_on_a_single_machine_example.cpp
 * @brief
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2013-09-17
 */

#include "paal/greedy/scheduling_jobs_with_deadlines_on_a_single_machine/scheduling_jobs_with_deadlines_on_a_single_machine.hpp"
#include "paal/utils/functors.hpp"

#include <boost/range/irange.hpp>

#include <iostream>
#include <utility>

using namespace paal::greedy::scheduling_jobs_with_deadlines_on_a_single_machine;
using namespace std;
using namespace paal::utils;
int main() {
//! [Scheduling Jobs Example]
    // sample data
    typedef double Time;
    vector<Time> time={2.1,3.1,4.1,5.1,6.1,7.1,8.1};
    std::vector<Time> relase={1,2,3,4,5,6,7};
    std::vector<Time> dueDate={-1,0,-2,-3,-4,-5,-6};

    auto jobs = boost::irange(0,int(time.size()));

    std::vector<std::pair<decltype(jobs)::iterator,Time> > jobsToStartDates;

    Time delay=schedulingJobsWithDeadlinesOnASingleMachine(
            jobs.begin(),jobs.end(),
            make_array_to_functor(time),
            make_array_to_functor(relase),
            make_array_to_functor(dueDate),
            back_inserter(jobsToStartDates));
    for(auto jobStartTime:jobsToStartDates){
        cout<<"Job "<<(*jobStartTime.first)<<" Start time: "<<jobStartTime.second<<endl;
    }
    //print result
    cout<<"Solution: "<<delay<<endl;
//! [Scheduling Jobs Example]
    return 0;

}
