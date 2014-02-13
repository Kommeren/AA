//=======================================================================
// Copyright (c) 2013 Piotr Smulewicz
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file scheduling_jobs_with_deadlines_on_a_single_machine_test.cpp
 * @brief
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2013-09-06
 */
#include "test_utils/logger.hpp"
#include "test_utils/test_result_check.hpp"

#include "paal/greedy/scheduling_jobs_with_deadlines_on_a_single_machine/scheduling_jobs_with_deadlines_on_a_single_machine.hpp"
#include "paal/utils/irange.hpp"

#include <boost/test/unit_test.hpp>

#include <vector>

BOOST_AUTO_TEST_CASE(scheduling_jobs_with_deadlines_on_a_single_machine) {
    // sample data
    using Time = double;
    std::vector<Time> time = { 2.1, 3.1, 4.1, 5.1, 6.1, 7.1, 8.1 };
    std::vector<Time> relase = { 1, 2, 3, 4, 5, 6, 7 };
    std::vector<Time> due_date = { -1, 0, -2, -3, -4, -5, -6 };
    double BEST_DELAY = 36.7;

    auto jobs = paal::irange(time.size());
    std::vector<std::pair<decltype(jobs)::iterator, Time>> jobs_to_start_dates;

    Time delay =
        paal::greedy::scheduling_jobs_with_deadlines_on_a_single_machine(
            jobs.begin(), jobs.end(), paal::utils::make_array_to_functor(time),
            paal::utils::make_array_to_functor(relase),
            paal::utils::make_array_to_functor(due_date),
            back_inserter(jobs_to_start_dates));
    Time max_delay = 0;
    int jobId;
    for (auto job_start_time : jobs_to_start_dates) {
        Time start_time = job_start_time.second;
        jobId = (*(job_start_time.first));
        LOGLN("Job " << jobId << " Start time: " << start_time);
        if ((start_time + time[jobId] - due_date[jobId]) > max_delay)
            max_delay = start_time + time[jobId] - due_date[jobId];
    }
    // print result
    check_result(delay, BEST_DELAY, 2);
    BOOST_CHECK_EQUAL(delay, max_delay);
}
