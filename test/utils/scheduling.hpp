//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file scheduling.hpp
 * @brief
 * @author Robert Rosolek
 * @version 1.0
 * @date 2013-11-19
 */
#ifndef SCHEDULING_UTILS
#define SCHEDULING_UTILS

#include "utils/logger.hpp"

#include "paal/utils/type_functions.hpp"

#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/max_element.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/functional/hash.hpp>

#include <unordered_map>
#include <utility>
#include <vector>

template <class Machines, class Time, class GetSpeed>
std::vector<long long> generate_job_loads(Machines machines,
                                          double minJobsOnMachine, Time time,
                                          GetSpeed getSpeed) {
    std::vector<long long> loads;
    for (const auto machine : machines) {
        for (Time left = time; left > 0;) {
            Time jobTime = rand() % ((long long)(time / minJobsOnMachine));
            jobTime = std::min(jobTime, left);
            loads.push_back(jobTime * getSpeed(machine));
            left -= jobTime;
        }
    }
    return loads;
}

template <class Result, class Job>
void check_jobs(Result result, std::vector<Job> jobs) {
    std::vector<Job> gotJobs;
    for (const auto &it : result) {
        gotJobs.push_back(*it.second);
    }
    std::sort(gotJobs.begin(), gotJobs.end());
    std::sort(jobs.begin(), jobs.end());
    BOOST_CHECK(jobs == gotJobs);
}

template <class Result, class GetSpeed>
double get_max_time(const Result &result, GetSpeed getSpeed) {
    typedef typename paal::range_to_elem_t<Result>::first_type MachineIter;
    typedef typename std::iterator_traits<MachineIter>::value_type Machine;
    std::unordered_map<Machine, double, boost::hash<Machine>> machineTime;
    for (const auto &machineJobPair : result) {
        Machine machine = *machineJobPair.first;
        auto load = *machineJobPair.second;
        machineTime[machine] += double(load) / getSpeed(machine);
    }
    return *boost::max_element(machineTime | boost::adaptors::map_values);
}

#endif /* SCHEDULING_JOBS_UTILS */
