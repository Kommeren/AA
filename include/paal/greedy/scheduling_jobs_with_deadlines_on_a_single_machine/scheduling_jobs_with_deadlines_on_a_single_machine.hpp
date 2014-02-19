/**
 * @file scheduling_jobs_with_deadlines_on_a_single_machine.hpp
 * @brief
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2013-09-09
 */
#ifndef SCHEDULING_JOBS_WITH_DEADLINES_ON_A_SINGLE_MACHINE
#define SCHEDULING_JOBS_WITH_DEADLINES_ON_A_SINGLE_MACHINE


#include <queue>
#include <vector>
#include <algorithm>
#include <utility>

#include <boost/iterator/counting_iterator.hpp>
#include "paal/utils/functors.hpp"
#include "paal/utils/type_functions.hpp"

namespace paal{
namespace greedy{
namespace scheduling_jobs_with_deadlines_on_a_single_machine{

/**
 * @brief solve scheduling jobs on identical parallel machines problem
 * and fill start time of all jobs
 * example:
 *  \snippet scheduling_jobs_with_deadlines_on_a_single_machine_example.cpp Scheduling Jobs Example
 *
 * complete example is scheduling_jobs_with_deadlines_on_a_single_machine_example.cpp
 * @param InputIterator first - jobs begin
 * @param InputIterator last - jobs end
 * @param GetTime getTime
 * @param GetReleaseDate getReleaseDate
 * @param GetDueDate getDueDate
 * @param OutputIterator result
 * @tparam Time
 * @tparam InputIterator
 * @tparam OutputIterator
 * @tparam GetTime
 * @tparam GetDueDate
 * @tparam GetReleaseDate
 */
template<class InputIterator, class OutputIterator, class GetTime,class GetDueDate,class GetReleaseDate>
auto schedulingJobsWithDeadlinesOnASingleMachine(
        const InputIterator first,const InputIterator last
        ,GetTime getTime,GetReleaseDate getReleaseDate,GetDueDate getDueDate
        ,OutputIterator result)-> puretype (getTime(*first)){
    typedef puretype( getTime(*first)) Time;
    std::vector<InputIterator> jobs;
    std::copy(boost::make_counting_iterator(first),
          boost::make_counting_iterator(last),
          std::back_inserter(jobs));

    auto getDueDateFromIterator=utils::make_LiftIteratorFunctor(getDueDate);
    auto dueDateCompatator=utils::make_FunctorToComparator(getDueDateFromIterator,utils::Greater());
    typedef std::priority_queue<InputIterator,std::vector<InputIterator>,decltype (dueDateCompatator)> QueueType;
    QueueType activeJobsIters(dueDateCompatator);

    auto getReleaseDateFromIterator=utils::make_LiftIteratorFunctor(getReleaseDate);
    std::sort(jobs.begin(),jobs.end(),utils::make_FunctorToComparator(getReleaseDateFromIterator));
    Time startIdle=Time();
    Time longestDelay=Time();
    auto doJob=[&](){
        auto jobIter=activeJobsIters.top();
        activeJobsIters.pop();
        Time startTime=std::max(startIdle,getReleaseDate(*jobIter));
        startIdle=startTime+getTime(*jobIter);
        longestDelay=std::max(longestDelay,startIdle-getDueDate(*jobIter));
        *result=std::make_pair(jobIter,startTime);
        ++result;
    };
    for(auto jobIter:jobs){
        while(!activeJobsIters.empty() && getReleaseDate(*jobIter)>startIdle )
            doJob();
        activeJobsIters.push(jobIter);
    }
    while(!activeJobsIters.empty()){
        doJob();
    }

    return longestDelay;
}
}//!scheduling_Jobs_with_deadlines_on_a_single_machine
}//!greedy
}//!paal

#endif /* SCHEDULING_JOBS_WITH_DEADLINES_ON_A_SINGLE_MACHINE */
