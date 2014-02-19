/**
 * @file scheduling_jobs_with_deadlines_on_a_single_machine_test.cpp
 * @brief
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2013-09-06
 */
#include <vector>
#include <boost/test/unit_test.hpp>
#include <boost/range/irange.hpp>
#include "utils/logger.hpp"
#include "paal/greedy/scheduling_jobs_with_deadlines_on_a_single_machine/scheduling_jobs_with_deadlines_on_a_single_machine.hpp"


BOOST_AUTO_TEST_CASE(scheduling_jobs_with_deadlines_on_a_single_machine) {
    // sample data
    typedef double Time;
    std::vector<Time> time={2.1,3.1,4.1,5.1,6.1,7.1,8.1};
    std::vector<Time> relase={1,2,3,4,5,6,7};
    std::vector<Time> dueDate={-1,0,-2,-3,-4,-5,-6};
    double bestDelay=36.7;

    auto jobs = boost::irange(0,int(time.size()));

    std::vector<std::pair<decltype(jobs)::iterator,Time> > jobsToStartDates;

    Time delay=paal::greedy::scheduling_jobs_with_deadlines_on_a_single_machine::schedulingJobsWithDeadlinesOnASingleMachine(
            jobs.begin(),jobs.end(),
            paal::utils::make_ArrayToFunctor(time),
            paal::utils::make_ArrayToFunctor(relase),
            paal::utils::make_ArrayToFunctor(dueDate),
            back_inserter(jobsToStartDates));
    Time maxDelay=0;
    int jobId;
    for(auto jobStartTime:jobsToStartDates){
        Time startTime=jobStartTime.second;
        jobId=(*(jobStartTime.first) );
        LOGLN("Job "<<jobId<<" Start time: "<<startTime);
        if((startTime+time[jobId]-dueDate[jobId])>maxDelay)
            maxDelay=startTime+time[jobId]-dueDate[jobId];
    }
    //print result
    LOGLN("Solution:");
    LOGLN(delay);
    LOGLN("Aproximation ratio:");
    LOGLN(delay/bestDelay);
    BOOST_CHECK(delay>=bestDelay);
    BOOST_CHECK(delay<=bestDelay*2);
    BOOST_CHECK_EQUAL(delay,maxDelay);
}
