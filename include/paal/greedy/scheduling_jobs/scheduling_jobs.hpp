/*
 * @file scheduling_jobs.hpp
 * @brief
 * @author Robert Rosolek
 * @version 1.0
 * @date 2013-11-19
 */
#ifndef SCHEDULING_JOBS
#define SCHEDULING_JOBS

#define BOOST_RESULT_OF_USE_DECLTYPE

#include <algorithm>
#include <cassert>
#include <iterator>
#include <numeric>
#include <random>
#include <utility>
#include <vector>

#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/algorithm/lower_bound.hpp>
#include <boost/range/algorithm/sort.hpp>
#include <boost/range/irange.hpp>
#include <boost/range/counting_range.hpp>
#include <boost/range/iterator_range.hpp>
#include <paal/data_structures/fraction.hpp>
#include <paal/utils/functors.hpp>
#include <paal/utils/type_functions.hpp>


namespace paal {
namespace greedy {

namespace detail {

   template<class MachineIterator, class JobIterator,
            class GetSpeed, class GetLoad>
   struct Types {
      typedef typename std::iterator_traits<MachineIterator>::reference MachineReference;
      typedef typename std::iterator_traits<JobIterator>::reference JobReference;
      typedef typename utils::pure_result_of<GetSpeed(MachineReference)>::type Speed;
      typedef typename utils::pure_result_of<GetLoad(JobReference)>::type Load;
      typedef data_structures::Fraction<Load, Speed> Frac;
   };

   template<class MachineIterator,
            class JobIterator,
            class GetSpeed,
            class GetLoad>
   typename Types<MachineIterator, JobIterator, GetSpeed, GetLoad>::Frac
   calculate_bound(const MachineIterator mFirst, const MachineIterator mLast,
                  const JobIterator jFirst, const JobIterator jLast,
                  GetSpeed getSpeed, GetLoad getLoad) {
      typedef Types<MachineIterator, JobIterator, GetSpeed, GetLoad> Traits;
      typedef typename Traits::Speed Speed;
      typedef typename Traits::Load Load;
      typedef typename Traits::Frac Frac;

      std::vector<Speed> speedSum;
      std::transform(mFirst, mLast, std::back_inserter(speedSum), getSpeed);
      std::partial_sum(speedSum.begin(), speedSum.end(), speedSum.begin());

      std::vector<Load> loadSum;
      std::transform(jFirst, jLast, std::back_inserter(loadSum), getLoad);
      std::partial_sum(loadSum.begin(), loadSum.end(), loadSum.begin());

      auto jobsNum = jLast - jFirst;
      auto machinesNum = mLast - mFirst;
      typedef decltype(machinesNum) MachinesNumType;
      assert(jobsNum > 0 && machinesNum > 0);
      Frac result(getLoad(*jFirst), getSpeed(*mFirst));
      for (auto jobID: boost::irange(static_cast<decltype(jobsNum)>(0), jobsNum)) {
         Load load = getLoad(jFirst[jobID]);
         auto getSingle = [=] (MachinesNumType i) { return Frac(load, getSpeed(mFirst[i])); };
         auto getSummed = [&] (MachinesNumType i) { return Frac(loadSum[jobID], speedSum[i]); };
         auto condition = [=] (MachinesNumType i) { return getSummed(i) < getSingle(i); };
         auto machinesIDs = boost::counting_range(static_cast<MachinesNumType>(0), machinesNum);
         auto it = boost::lower_bound(
            machinesIDs | boost::adaptors::transformed(utils::make_assignable_functor(condition)),
            true
         ).base();
         MachinesNumType machineID = (it != machinesIDs.end()) ? *it : machinesNum - 1;
         auto getMax = [=] (MachinesNumType i) { return std::max(getSingle(i), getSummed(i)); };
         Frac candidate = getMax(machineID);
         if (machineID != 0) {
            candidate = std::min(candidate, getMax(machineID - 1));
         }
         result = std::max(result, candidate);
      }
      return result;
   }

   template<class MachineIterator,
            class JobIterator,
            class OutputIterator,
            class GetSpeed,
            class GetLoad,
            class RoundFun>
   void schedule(MachineIterator mFirst, MachineIterator mLast,
         JobIterator jFirst, JobIterator jLast,
         OutputIterator result, GetSpeed getSpeed, GetLoad getLoad, RoundFun round) {
      typedef Types<MachineIterator, JobIterator, GetSpeed, GetLoad> Traits;
      typedef typename Traits::MachineReference MachineReference;
      typedef typename Traits::JobReference JobReference;
      typedef typename Traits::Speed Speed;
      typedef typename Traits::Load Load;

      if (mFirst == mLast || jFirst == jLast) {
         return;
      }

      std::vector<MachineIterator> machines;
      boost::copy(boost::counting_range(mFirst, mLast), std::back_inserter(machines));
      auto getSpeedFromIterator = utils::make_lift_iterator_functor(getSpeed);
      boost::sort(machines, utils::make_functor_to_comparator(getSpeedFromIterator, utils::Greater()));

      std::vector<JobIterator> jobs;
      boost::copy(boost::counting_range(jFirst, jLast), std::back_inserter(jobs));
      auto getLoadFromIterator = utils::make_lift_iterator_functor(getLoad);
      boost::sort(jobs, utils::make_functor_to_comparator(getLoadFromIterator, utils::Greater()));

      auto bound = detail::calculate_bound(machines.begin(), machines.end(), jobs.begin(), jobs.end(),
         getSpeedFromIterator, getLoadFromIterator);
      Load boundLoad = bound.num;
      Speed boundSpeed = bound.den;
      Load currentLoad = 0;
      auto emit = [&result] (MachineIterator mIter, JobIterator jIter) {
         *result = std::make_pair(mIter, jIter);
         ++result;
      };
      auto jobIter = jobs.begin();
      for (auto machineIter = machines.begin(); machineIter != machines.end(); ++machineIter) {
         MachineReference machine = *(*machineIter);
         Speed speed = getSpeed(machine);
         while (jobIter != jobs.end()) {
            JobReference job = *(*jobIter);
            Load jobLoad = getLoad(job) * boundSpeed,
               newLoad = currentLoad + jobLoad;
            assert(newLoad <= boundLoad * (2 * speed));
            if (boundLoad * speed < newLoad) {
               Load fracLoad = boundLoad * speed - currentLoad;
               if (round(fracLoad, jobLoad)) {
                  emit(*machineIter, *jobIter);
               }
               else {
                  auto nextMachineIter = std::next(machineIter);
                  assert(nextMachineIter != machines.end());
                  emit(*nextMachineIter, *jobIter);
               }
               ++jobIter;
               currentLoad = jobLoad - fracLoad;
               break;
            }
            emit(*machineIter, *jobIter);
            ++jobIter;
            currentLoad = newLoad;
         }
      }
      assert(jobIter == jobs.end());
   }
} //!detail

/*
 * @brief this is deterministic solve scheduling jobs on machines with different
 * speeds problem and return schedule example:
 *  \snippet scheduling_example.cpp Scheduling Jobs Example
 *
 * complete example is scheduling_jobs_example.cpp
 * @param MachineIterator mFirst
 * @param MachineIterator mLast
 * @param JobIterator jFirst
 * @param JobIterator jLast
 * @param OutputIterator result
 * @param GetSpeed getSpeed
 * @param GetLoad getLoad
 * @tparam MachineIterator
 * @tparam JobIterator
 * @tparam OutputIterator
 * @tparam GetSpeed
 * @tparam GetLoad
 */
template<class MachineIterator,
   class JobIterator,
   class OutputIterator,
   class GetSpeed,
   class GetLoad>
void schedule_deterministic(const MachineIterator mFirst, const MachineIterator mLast,
   const JobIterator jFirst, const JobIterator jLast,
   OutputIterator result, GetSpeed getSpeed, GetLoad getLoad) {
   detail::schedule(mFirst, mLast, jFirst, jLast, result, getSpeed, getLoad, utils::always_true());
}

/*
 * @brief this is randomized solve scheduling jobs on machines with different
 * speeds problem and return schedule example:
 *  \snippet scheduling_example.cpp Scheduling Jobs Example
 *
 * complete example is scheduling_jobs_example.cpp
 * @param MachineIterator mFirst
 * @param MachineIterator mLast
 * @param JobIterator jFirst
 * @param JobIterator jLast
 * @param OutputIterator result
 * @param GetSpeed getSpeed
 * @param GetLoad getLoad
 * @param RandomNumberGenerator gen
 * @tparam MachineIterator
 * @tparam JobIterator
 * @tparam OutputIterator
 * @tparam GetSpeed
 * @tparam GetLoad
 * @tparam RandomNumberGenerator
 */
template<class MachineIterator,
   class JobIterator,
   class OutputIterator,
   class GetSpeed,
   class GetLoad,
   class RandomNumberGenerator>
void schedule_randomized(const MachineIterator mFirst, const MachineIterator mLast,
      const JobIterator jFirst, const JobIterator jLast,
      OutputIterator result, GetSpeed getSpeed, GetLoad getLoad, RandomNumberGenerator&& gen) {
   typedef typename detail::Types<MachineIterator, JobIterator, GetSpeed, GetLoad> Traits;
   double alpha = std::uniform_real_distribution<double>()(gen);
   auto round = [alpha](typename Traits::Load fractionalLoad, typename Traits::Load totalLoad) {
      return totalLoad * alpha < fractionalLoad;
   };
   detail::schedule(mFirst, mLast, jFirst, jLast, result, getSpeed, getLoad, round);
}

/*
 * @brief this is randomized (with default random engine) solve scheduling jobs
 * on machines with different speeds problem and return schedule example:
 *  \snippet scheduling_example.cpp Scheduling Jobs Example
 *
 * complete example is scheduling_jobs_example.cpp
 * @param MachineIterator mFirst
 * @param MachineIterator mLast
 * @param JobIterator jFirst
 * @param JobIterator jLast
 * @param OutputIterator result
 * @param GetSpeed getSpeed
 * @param GetLoad getLoad
 * @param RandomNumberGenerator gen
 * @tparam MachineIterator
 * @tparam JobIterator
 * @tparam OutputIterator
 * @tparam GetSpeed
 * @tparam GetLoad
 * @tparam RandomNumberGenerator
 */
template<class MachineIterator,
   class JobIterator,
   class OutputIterator,
   class GetSpeed,
   class GetLoad>
void schedule_randomized(const MachineIterator mFirst, const MachineIterator mLast,
      const JobIterator jFirst, const JobIterator jLast,
      OutputIterator result, GetSpeed getSpeed, GetLoad getLoad) {
   schedule_randomized(mFirst, mLast, jFirst, jLast, result, getSpeed, getLoad, std::default_random_engine(97345631u));
}

}//!greedy
}//!paal

#endif /* SCHEDULING_JOBS */
