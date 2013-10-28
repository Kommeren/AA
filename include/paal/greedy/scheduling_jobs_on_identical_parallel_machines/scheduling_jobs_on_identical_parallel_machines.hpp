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
template<class _RandomAccessIter, class OutputIterator,class GetTime>
void schedulingJobsOnIdenticalParallelMachines(int n_machines,const _RandomAccessIter first,const _RandomAccessIter last, OutputIterator result,GetTime getTime){
    typedef typename std::iterator_traits<_RandomAccessIter>::reference JobReference;
    typedef typename utils::PureResultOf<GetTime(JobReference)>::type Time;
    std::sort(first,last,utils::Greater());
    std::priority_queue<std::pair<Time,int> > machines;
    for(auto machineId : boost::irange(0, n_machines)){
        machines.push(std::make_pair(0,machineId));
    }
    for(JobReference job: boost::make_iterator_range(first,last)){
        auto leastLoadedMachine=machines.top();
        machines.pop();
        machines.push(std::make_pair(leastLoadedMachine.first-getTime(job),leastLoadedMachine.second));
        *result = std::make_pair(leastLoadedMachine.second,job);
        ++result;
    }
}
}//!scheduling_jobs_on_identical_parallel_machines
}//!greedy
}//!paal

#endif /* SCHEDULING_JOBS_ON_IDENTICAL_PARALLEL_MACHINES */