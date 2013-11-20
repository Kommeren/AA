/**
 * @file scheduling_jobs_on_identical_parallel_machines.hpp
 * @brief 
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2013-09-06
 */
#ifndef SCHEDULING_JOBS_ON_IDENTICAL_PARALLEL_MACHINES
#define SCHEDULING_JOBS_ON_IDENTICAL_PARALLEL_MACHINES

#include <queue>
#include <vector>
#include <algorithm>
#include <utility>
#include <boost/range/irange.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/iterator/counting_iterator.hpp>
#include <paal/utils/functors.hpp>
#include <paal/utils/type_functions.hpp>


namespace paal{
namespace greedy{
namespace scheduling_jobs_on_identical_parallel_machines{
    
/**
 * @brief this is solve scheduling jobs on identical parallel machines problem
 * and return schedule
 * example: 
 *  \snippet scheduling_jobs_on_identical_parallel_machines_example.cpp Scheduling Jobs Example
 *
 * complete example is scheduling_jobs_on_identical_parallel_machines_example.cpp
 * @param int n_machines
 * @param _RandomAccessIter first
 * @param _RandomAccessIter last
 * @param OutputIterator result
 * @tparam _RandomAccessIter
 */
template<class InputIterator, class OutputIterator,class GetTime>
void schedulingJobsOnIdenticalParallelMachines(int n_machines,const InputIterator first,const InputIterator last, OutputIterator result,GetTime getTime){
    typedef typename std::iterator_traits<InputIterator>::reference JobReference;
    typedef typename utils::PureResultOf<GetTime(JobReference)>::type Time;
    
    std::vector<InputIterator> jobs;
    std::copy(boost::make_counting_iterator(first),
          boost::make_counting_iterator(last),
          std::back_inserter(jobs));
    auto getTimeFromIterator=utils::make_LiftIteratorFunctor(getTime);
    std::sort(jobs.begin(),jobs.end(),utils::make_FunctorToComparator(getTimeFromIterator,utils::Greater()));
    
    //std::sort(first,last,utils::Greater());
    std::priority_queue<std::pair<Time,int> > machines;
    for(auto machineId : boost::irange(0, n_machines)){
        machines.push(std::make_pair(0,machineId));
    }
    for(auto jobIter: jobs){
        auto leastLoadedMachine=machines.top();
        machines.pop();
        machines.push(std::make_pair(leastLoadedMachine.first-getTime(*jobIter),leastLoadedMachine.second));
        *result = std::make_pair(leastLoadedMachine.second,jobIter);
        ++result;
    }
}
}//!scheduling_jobs_on_identical_parallel_machines
}//!greedy
}//!paal

#endif /* SCHEDULING_JOBS_ON_IDENTICAL_PARALLEL_MACHINES */
