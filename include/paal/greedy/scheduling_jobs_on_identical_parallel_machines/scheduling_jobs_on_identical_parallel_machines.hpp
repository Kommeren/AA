/**
 * @file scheduling_jobs_on_identical_parallel_machines.hpp
 * @brief
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2013-09-06
 */
#ifndef SCHEDULING_JOBS_ON_IDENTICAL_PARALLEL_MACHINES
#define SCHEDULING_JOBS_ON_IDENTICAL_PARALLEL_MACHINES

#include <boost/range/irange.hpp>

#include <queue>
#include <vector>
#include <algorithm>
#include <utility>
#include <paal/utils/functors.hpp>
#include <paal/utils/type_functions.hpp>

namespace paal {
namespace greedy {
namespace detail {
class compare {
  public:
    compare(std::vector<int> &load) : m_load(load) {}
    bool operator()(int lhs, int rhs) const {
        return m_load[lhs] < m_load[rhs];
    }

  private:
    const std::vector<int> &m_load;
};
} //!detail

/**
 * @brief this is solve scheduling jobs on identical parallel machines problem
 * and return schedule
 * example:
 *  \snippet scheduling_jobs_on_identical_parallel_machines_example.cpp Scheduling Jobs On Identical Parallel Machines Example
 *
 * complete example is
 * scheduling_jobs_on_identical_parallel_machines_example.cpp
 * @param n_machines
 * @param first
 * @param last
 * @param result
 * @param get_time
 */
template <class InputIterator, class OutputIterator, class GetTime>
void scheduling_jobs_on_identical_parallel_machines(int n_machines,
                                                    InputIterator first,
                                                    InputIterator last,
                                                    OutputIterator result,
                                                    GetTime get_time) {
    using JobReference =
        typename std::iterator_traits<InputIterator>::reference;
    using Time = typename utils::pure_result_of<GetTime(JobReference)>::type;

    std::sort(first, last, utils::Greater());
    std::vector<int> load(n_machines);

    std::priority_queue<int, std::vector<int>, detail::compare> machines(load);

    for (auto machine_id : boost::irange(0, n_machines)) {
        machines.push(machine_id);
    }
    for (auto job_iter = first; job_iter < last; job_iter++) {
        int least_loaded_machine = machines.top();
        machines.pop();
        load[least_loaded_machine] -= get_time(*job_iter);
        machines.push(least_loaded_machine);
        *result = std::make_pair(least_loaded_machine, job_iter);
        ++result;
    }
}

} //!greedy
} //!paal

#endif /* SCHEDULING_JOBS_ON_IDENTICAL_PARALLEL_MACHINES */
