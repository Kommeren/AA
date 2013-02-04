#define BOOST_TEST_MODULE two_local_search_test

#include <boost/test/unit_test.hpp>

#include <vector>
#include <string>

#include "TSPLIB/TSPLIB.h"
#include "local_search/2_local_search/2_local_search.hpp"
#include "simple_algo/cycle_algo.hpp"
#include "utils/sample_graph.hpp"

using std::string;
using std::vector;
using namespace  paal::local_search::two_local_search;
using namespace  paal;


BOOST_AUTO_TEST_CASE(two_local_search_test) {
//! [Two Local Search Example]
   //sample data
   typedef  SampleGraphsMetrics SGM;
   auto gm = SGM::getGraphMetric();
   const int size = SGM::getSize();
   std::vector<int> v(size);
   std::iota(v.begin(), v.end(), 0);

   //create random solution 
   std::random_shuffle(v.begin(), v.end());
   data_structures::SimpleCycle<int> cycle(v.begin(), v.end());

   //creating local search
   ImproveChecker2Opt<decltype(gm)> ch(gm);
   auto ls = make_TwoLocalSearchStep(std::move(cycle), ch);

   //printing 
   auto const & cman = ls.getSolution();
   std::cout << "Length \t" << simple_algo::getLength(gm, cman) << std::endl;
   int i = 0;

   //search
   while(ls.search()) {
       //printing
       std::cout << "Length after\t" << i++ << ": " 
                 << simple_algo::getLength(gm, cman) << std::endl;
   }
//! [Two Local Search Example]
}

