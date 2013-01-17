#define BOOST_TEST_MODULE cycle_manager

#include <boost/test/unit_test.hpp>

#include <vector>
#include <string>

#include "TSPLIB/TSPLIB.h"
#include "local_search/2_local_search/2_local_search.hpp"
#include "simple_algo/cycle_algo.hpp"

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

        std::random_shuffle(v.begin(), v.end());
        TwoLocalSearchStep<int, tsp::TSPLIB_Matrix> ls(v.begin(), v.end(), mtx);
        auto const & cman = ls.getCycle();
        std::cout << "Graph:\t" << g.filename << std::endl;
        std::cout << "Length before\t" << simple_algo::getLength(mtx, cman) << std::endl;
        int i = 0;
        while(ls.search()) {
            std::cout << "Length after\t" << i++ << ": " << simple_algo::getLength(mtx, cman) << std::endl;
        }
   }

}

