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
#include <paal/utils/functors.hpp>
#include <paal/utils/type_functions.hpp>


namespace paal{
namespace greedy{

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
void scheduling_jobs_on_identical_parallel_machines(int n_machines,InputIterator first,InputIterator last, OutputIterator result,GetTime getTime){
    typedef typename std::iterator_traits<InputIterator>::reference JobReference;
    typedef typename utils::pure_result_of<GetTime(JobReference)>::type Time;

    std::sort(first,last,utils::Greater());
    std::priority_queue<std::pair<Time,int> > machines;
    for(auto machineId : boost::irange(0, n_machines)){
        machines.push(std::make_pair(0,machineId));
    }
    for(auto jobIter=first;jobIter<last;jobIter++){
        auto leastLoadedMachine=machines.top();
        machines.pop();
        machines.push(std::make_pair(leastLoadedMachine.first-getTime(*jobIter),leastLoadedMachine.second));
        *result = std::make_pair(leastLoadedMachine.second,jobIter);
        ++result;
    }
}

}//!greedy
}//!paal

#endif /* SCHEDULING_JOBS_ON_IDENTICAL_PARALLEL_MACHINES */
