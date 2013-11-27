/**
 * @file scheduling_jobs_example.cpp
 * @brief
 * @author Robert Rosolek
 * @version 1.0
 * @date 2013-11-19
 */
#include <iostream>
#include <map>
#include <utility>

#include "paal/greedy/scheduling_jobs/scheduling_jobs.hpp"
#include "paal/utils/functors.hpp"

//! [Scheduling Jobs Example]
typedef double Time;
typedef std::pair<Time, char> Job;
typedef int Machine;

template <class Result>
void printResult(const Result& result) {
   std::map<Machine, Time> machineTime;
   for (const auto& machineJobpair: result) {
      Machine machine = *machineJobpair.first;
      Job job = *machineJobpair.second;
      machineTime[machine] += job.first / machine;
      std::cout << "On machine: " << machine << " do job: " << job.second << std::endl;
   }
   Time maxTime = 0;
   for (const auto& it : machineTime) {
      maxTime = std::max(maxTime, it.second);
   }
   std::cout << "Solution: " << maxTime << std::endl;
}

int main() {
   auto returnJobLoadFunctor = [](Job job) { return job.first; };

   std::vector<Machine> machines = {1, 2, 3};
   std::vector<Job> jobs = {{2.1,'a'}, {3.1,'b'}, {4.1,'c'}, {5.1,'d'}, {6.1,'e'},
      {7.1,'f'}, {8.1,'g'}};

   std::vector<std::pair<decltype(machines)::iterator, decltype(jobs)::iterator>>
      deterministicResult, randomizedResult;

   std::cout << "Deterministic schedule:" << std::endl;
   paal::greedy::scheduleDeterministic(machines.begin(), machines.end(),
      jobs.begin(), jobs.end(), back_inserter(deterministicResult),
      paal::utils::IdentityFunctor(), returnJobLoadFunctor);
   printResult(deterministicResult);

   std::cout << "Randomized schedule:" << std::endl;
   paal::greedy::scheduleRandomized(machines.begin(), machines.end(),
      jobs.begin(), jobs.end(), back_inserter(randomizedResult),
      paal::utils::IdentityFunctor(), returnJobLoadFunctor);
   printResult(randomizedResult);

   return 0;
}
//! [Scheduling Jobs Example]}
