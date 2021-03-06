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
#ifndef PAAL_SCHEDULING_HPP
#define PAAL_SCHEDULING_HPP

#include "test_utils/logger.hpp"

#include "paal/utils/type_functions.hpp"
#include "paal/utils/assign_updates.hpp"

#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/max_element.hpp>
#include <boost/range/algorithm/sort.hpp>
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
    for (auto const machine : machines) {
        for (Time left = time; left > 0;) {
            Time jobTime = rand() % ((long long)(time / minJobsOnMachine));
            paal::assign_min(jobTime, left);
            loads.push_back(jobTime * getSpeed(machine));
            left -= jobTime;
        }
    }
    return loads;
}

template <class Result, class Job>
void check_jobs(Result result, std::vector<Job> jobs) {
    std::vector<Job> gotJobs;
    for (auto const &it : result) {
        gotJobs.push_back(*it.second);
    }
    boost::sort(gotJobs);
    boost::sort(jobs);
    BOOST_CHECK(jobs == gotJobs);
}

template <class Result, class GetSpeed>
double get_max_time(const Result &result, GetSpeed getSpeed) {
    typedef typename paal::range_to_elem_t<Result>::first_type MachineIter;
    typedef typename std::iterator_traits<MachineIter>::value_type Machine;
    std::unordered_map<Machine, double, boost::hash<Machine>> machineTime;
    for (auto const &machineJobPair : result) {
        Machine machine = *machineJobPair.first;
        auto load = *machineJobPair.second;
        machineTime[machine] += double(load) / getSpeed(machine);
    }
    return *boost::max_element(machineTime | boost::adaptors::map_values);
}

#endif // PAAL_SCHEDULING_HPP
