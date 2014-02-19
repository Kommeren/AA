/**
 * @file scheduling_jobs_on_identical_parallel_machines_test.cpp
 * @brief
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2013-09-06
 */

#include <vector>
#include <boost/test/unit_test.hpp>
#include "paal/greedy/scheduling_jobs_on_identical_parallel_machines/scheduling_jobs_on_identical_parallel_machines.hpp"
#include "utils/logger.hpp"
#include "utils/scheduling.hpp"


BOOST_AUTO_TEST_CASE(scheduling_jobs_on_identical_parallel_machines) {
    // sample data
    int numberOfMachines = 3;
    typedef double Time;
    std::vector<Time> j={2.1,3.1,4.1,5.1,6.1,7.1,8.1};
    std::vector< std::pair<int, decltype(j)::iterator> > result;
    paal::greedy::schedulingJobsOnIdenticalParallelMachines(numberOfMachines,j.begin(),j.end(),back_inserter(result),paal::utils::IdentityFunctor());
    checkJobs(result, j);
    std::vector<Time> sumOfMachine;
    sumOfMachine.resize(numberOfMachines);
    for(auto jobMachinePair:result){
        sumOfMachine[jobMachinePair.first]+=*jobMachinePair.second;
    }

    Time maximumLoad=*std::max_element(sumOfMachine.begin(),sumOfMachine.end());

    Time sumAllLoads=std::accumulate(sumOfMachine.begin(),sumOfMachine.end(),0.0);
    //print result

    LOGLN("Solution:");
    LOGLN(maximumLoad);
    LOGLN("Upper Bound Aproxmimation Ratio:");
    auto upperBoundAproxmiationRatio=(maximumLoad*numberOfMachines)/double(sumAllLoads);
    LOGLN(upperBoundAproxmiationRatio);
    LOGLN("");
    BOOST_CHECK(upperBoundAproxmiationRatio<=4.0/3.0);
    BOOST_CHECK((maximumLoad*numberOfMachines)/double(sumAllLoads)>=1);

}
