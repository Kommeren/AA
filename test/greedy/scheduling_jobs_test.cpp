/**
 * @file scheduling_jobs_test.cpp
 * @brief
 * @author Robert Rosolek
 * @version 1.0
 * @date 2013-11-19
 */
#include "utils/scheduling.hpp"
#include "utils/test_result_check.hpp"

#include "paal/greedy/scheduling_jobs/scheduling_jobs.hpp"
#include "paal/utils/functors.hpp"

#include <boost/test/unit_test.hpp>

#include <vector>

using std::pair;
using std::vector;
using paal::utils::identity_functor;
using namespace paal::greedy;

typedef double Time;
typedef double Job;
typedef int Machine;

namespace {
   const vector<Machine> input_machines = {10, 20, 30};
   // comments show optimal grouping
   const vector<Job> input_jobs = {2, 2, 6,/* # */ 1, 2, 4, 6, 7,/* # */ 1, 1, 2, 3, 3, 3, 5, 12};
   const Time opt = 1;
}

BOOST_AUTO_TEST_CASE(testDeterministicRounding) {
   vector<Job> jobs = input_jobs;
   vector<Machine> machines = input_machines;
   vector<pair<decltype(machines)::iterator, decltype(jobs)::iterator>> result;
   schedule_deterministic(machines.begin(), machines.end(), jobs.begin(), jobs.end(),
      back_inserter(result), identity_functor(), identity_functor());
   check_jobs(result, jobs);
   Time max_time = get_max_time(result, identity_functor());
   check_result(max_time,opt,2.0);
}

BOOST_AUTO_TEST_CASE(testRandomizedRounding) {
   vector<Job> jobs = input_jobs;
   vector<Machine> machines = input_machines;
   vector<std::pair<decltype(machines)::iterator, decltype(jobs)::iterator>> result;
   schedule_randomized(machines.begin(), machines.end(), jobs.begin(), jobs.end(),
      back_inserter(result), identity_functor(), identity_functor());
   check_jobs(result, jobs);
   Time max_time = get_max_time(result, identity_functor());
   check_result(max_time,opt,2.0);
}
