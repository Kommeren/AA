/**
 * @file scheduling.hpp
 * @brief
 * @author Robert Rosolek
 * @version 1.0
 * @date 2013-11-19
 */
#ifndef SCHEDULING_UTILS
#define SCHEDULING_UTILS

#include <map>
#include <utility>
#include <vector>

#include "utils/logger.hpp"
#include "paal/utils/type_functions.hpp"
#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/max_element.hpp>
#include <boost/test/unit_test.hpp>

template <class Machines, class Time, class GetSpeed>
std::vector<long long> generateJobLoads(Machines machines,
   double minJobsOnMachine, Time time, GetSpeed getSpeed){
   std::vector<long long> loads;
   for (const auto machine: machines) {
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
void checkJobs(Result result, std::vector<Job> jobs) {
   std::vector<Job> gotJobs;
   for (const auto& it: result) {
      gotJobs.push_back(*it.second);
   }
   std::sort(gotJobs.begin(), gotJobs.end());
   std::sort(jobs.begin(), jobs.end());
   BOOST_CHECK(jobs == gotJobs);
}

template <class Result, class GetSpeed>
double getMaxTime(const Result& result, GetSpeed getSpeed) {
   typedef typename paal::utils::CollectionToElem<Result>::type::first_type MachineIter;
   typedef typename std::iterator_traits<MachineIter>::value_type Machine; 
   std::map<Machine, double> machineTime;
   for (const auto& machineJobPair: result) {
      Machine machine = *machineJobPair.first;
      auto load = *machineJobPair.second;
      machineTime[machine] += double(load) / getSpeed(machine);
   }
   return *boost::max_element(machineTime | boost::adaptors::map_values);
}

#endif /* SCHEDULING_JOBS_UTILS */
