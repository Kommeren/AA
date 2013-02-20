#define BOOST_TEST_MODULE two_local_search_long

#include <boost/test/unit_test.hpp>

#include <vector>
#include <string>

#include "TSPLIB/TSPLIB.h"
#include "paal/local_search/2_local_search/2_local_search.hpp"
#include "paal/simple_algo/cycle_algo.hpp"
#include "utils/logger.hpp"
#include "paal/data_structures/simple_cycle.hpp"

using std::string;
using std::vector;
using namespace  paal::local_search::two_local_search;
using namespace  paal;


std::string path = "test/TSPLIB/TSPLIB/symmetrical";


BOOST_AUTO_TEST_CASE(TSPLIB) {
   tsp::TSPLIB_Directory dir(path);
   tsp::TSPLIB_Matrix mtx;
   for(auto &g : dir.graphs)  {   
        g.load(mtx);
        auto size = mtx.size1();
        std::vector<int> v(size);
        std::iota(v.begin(), v.end(), 0);

        //create random solution 
        std::random_shuffle(v.begin(), v.end());
        data_structures::SimpleCycle<int> cycle(v.begin(), v.end());

        //creating local search
        auto lsc = getDefaultTwoLocalComponents(mtx);
        auto ls = TwoLocalSearchStep<decltype(cycle), decltype(lsc)>(cycle, lsc);

        //printing 

#ifdef LOGGER_ON
        auto const & cman = ls.getSolution();
        LOG("Graph:\t" << g.filename);
        LOG("Length before\t" << simple_algo::getLength(mtx, cman));
        int i = 0;
#endif

        //search
        while(ls.search()) {
            LOG("Length after\t" << i++ << ": " << simple_algo::getLength(mtx, cman));
        }
   }

}

