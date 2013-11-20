/**
 * @file scheduling_jobs_test.cpp
 * @brief
 * @author Robert Rosolek
 * @version 1.0
 * @date 2013-11-19
 */
#include <vector>

#include <boost/test/unit_test.hpp>

#include "paal/greedy/scheduling_jobs/scheduling_jobs.hpp"
#include "paal/utils/functors.hpp"
#include "utils/scheduling.hpp"

using std::pair;
using std::vector;
using paal::utils::IdentityFunctor;
using namespace paal::greedy;

typedef double Time;
typedef double Job;
typedef int Machine;

namespace {
   const vector<Machine> inputMachines = {10, 20, 30};
   // comments show optimal grouping
   const vector<Job> inputJobs = {2, 2, 6,/* # */ 1, 2, 4, 6, 7,/* # */ 1, 1, 2, 3, 3, 3, 5, 12};
   const Time opt = 1;
}

BOOST_AUTO_TEST_CASE(testDeterministicRounding) {
   vector<Job> jobs = inputJobs;
   vector<Machine> machines = inputMachines;
   vector<pair<decltype(machines)::iterator, decltype(jobs)::iterator>> result;
   scheduleDeterministic(machines.begin(), machines.end(), jobs.begin(), jobs.end(),
      back_inserter(result), IdentityFunctor(), IdentityFunctor());
   checkJobs(result, jobs);
   Time maxTime = getMaxTime(result, IdentityFunctor());
   LOGLN("Solution: " << maxTime);
   LOGLN("Approximation ratio: " << (maxTime / opt));
   BOOST_CHECK(maxTime <= 2 * opt);
}

BOOST_AUTO_TEST_CASE(testRandomizedRounding) {
   vector<Job> jobs = inputJobs;
   vector<Machine> machines = inputMachines;
   vector<std::pair<decltype(machines)::iterator, decltype(jobs)::iterator>> result;
   scheduleRandomized(machines.begin(), machines.end(), jobs.begin(), jobs.end(),
      back_inserter(result), IdentityFunctor(), IdentityFunctor());
   checkJobs(result, jobs);
   Time maxTime = getMaxTime(result, IdentityFunctor());
   LOGLN("Solution: " << maxTime);
   LOGLN("Approximation ratio: " << (maxTime / opt));
   BOOST_CHECK(maxTime <= 2 * opt);
}
