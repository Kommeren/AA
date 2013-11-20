/**
 * @file scheduling_jobs_long_test.cpp
 * @brief
 * @author Robert Rosolek
 * @version 1.0
 * @date 2013-11-19
 */
#include <vector>
#include <utility>

#include "paal/greedy/scheduling_jobs/scheduling_jobs.hpp"
#include "utils/scheduling.hpp"
#include "utils/logger.hpp"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(testSchedulingJobs) {
   typedef long long Time;
   typedef std::pair<int, long long> Machine;
   typedef Time Job;

   const unsigned seed = 68;
   const Time optTime = 10000;
   const long minMachines = 10;
   const long maxMachines = 100000;
   const long stepMachines = 10;
   const long maxMachineSpeed = 100;
   const double minJobsOnMachineStart = 1.0;
   const double minJobsOnMachineEnd = 5.0;
   const double minJobsOnMachineStep = 0.33;

   std::srand(seed);
   for (int numberOfMachines = minMachines;
         numberOfMachines <= maxMachines;
         numberOfMachines *= stepMachines) {
      for (double minJobsOnMachine = minJobsOnMachineStart;
            minJobsOnMachine < minJobsOnMachineEnd;
            minJobsOnMachine += minJobsOnMachineStep){
         LOGLN("machines: " << numberOfMachines);
         std::vector<Machine> machines(numberOfMachines);
         for (auto machineID: boost::irange(0, numberOfMachines)) {
            machines[machineID] = std::make_pair(machineID, rand() % maxMachineSpeed + 1);
         }
         auto getSpeed = [](Machine machine) { return machine.second; };

         std::vector<Job> jobs = generateJobLoads(machines,
               minJobsOnMachine, optTime, getSpeed);
         LOGLN("jobs: " << jobs.size());

         typedef std::vector<std::pair<decltype(machines)::iterator,
            decltype(jobs)::iterator>> Result;
         Result resultRandomized, resultDeterministic;

         paal::greedy::scheduleRandomized(machines.begin(), machines.end(),
               jobs.begin(), jobs.end(), back_inserter(resultRandomized),
               getSpeed, paal::utils::IdentityFunctor());
         paal::greedy::scheduleDeterministic(machines.begin(), machines.end(),
            jobs.begin(), jobs.end(), back_inserter(resultDeterministic),
               getSpeed, paal::utils::IdentityFunctor());

         auto checkAndPrint = [&] (const Result& result) {
            checkJobs(result, jobs);
            double maxTime = getMaxTime(result, getSpeed);
            LOGLN("Solution: " << maxTime);
            LOGLN("Approximation ratio: " << (double(maxTime) / optTime));
            LOGLN("");
            BOOST_CHECK(maxTime <= 2 * optTime);
         };
         checkAndPrint(resultRandomized);
         checkAndPrint(resultDeterministic);
      }
   }
}

