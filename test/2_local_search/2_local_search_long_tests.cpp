#define BOOST_TEST_MODULE two_local_search_long

#include <boost/test/unit_test.hpp>

#include <vector>
#include <string>

#include "paal/local_search/2_local_search/2_local_search.hpp"
#include "paal/data_structures/cycle/cycle_algo.hpp"
#include "paal/data_structures/cycle/simple_cycle.hpp"
#include "paal/local_search/components.hpp"

#include "utils/logger.hpp"
#include "utils/read_tsplib.h"

using std::string;
using std::vector;
using namespace paal::local_search::two_local_search;
using namespace paal;


std::string path = "test/data/TSPLIB/symmetrical/";


BOOST_AUTO_TEST_CASE(TSPLIB) {
    read_tsplib::TSPLIB_Directory dir(path);
    read_tsplib::TSPLIB_Matrix mtx;
    std::string fname;
    float opt;
    while(dir.getGraph(fname ,opt))  {  
        std::ifstream is(fname);
        read_tsplib::TSPLIB_Directory::Graph g(is);
        g.load(mtx);
        auto size = mtx.size();
        std::vector<int> v(size);
        std::iota(v.begin(), v.end(), 0);

        //create random solution 
        std::random_shuffle(v.begin(), v.end());
        data_structures::SimpleCycle<int> cycle(v.begin(), v.end());

        //creating local search
        auto lsc = getDefaultTwoLocalComponents(mtx);
        auto ls = TwoLocalSearchStep<decltype(cycle), decltype(lsc)>(std::move(cycle), std::move(lsc));

        //printing 
        ON_LOG(auto const & cman = ls.getSolution());
        LOG("Graph:\t" << fname);
        LOG("Length before\t" << simple_algo::getLength(mtx, cman));
        ON_LOG(int i = 0);

        //search
        while(ls.search()) {
            LOG("Length after\t" << i++ << ": " << simple_algo::getLength(mtx, cman));
        }
    }
}

using paal::local_search::SearchComponents;

BOOST_AUTO_TEST_CASE(TSPLIB_long) {
    const string indexFName = "index.long";
    read_tsplib::TSPLIB_Directory dir(path + "/long/", indexFName);
    read_tsplib::TSPLIB_Matrix mtx;
    std::string fname;
    float opt;
    while(dir.getGraph(fname ,opt))  {   
        std::ifstream is(fname);
        read_tsplib::TSPLIB_Directory::Graph g(is);
        g.load(mtx);
        auto size = mtx.size();
        std::vector<int> v(size);
        std::iota(v.begin(), v.end(), 0);

        //create random solution 
        std::random_shuffle(v.begin(), v.end());
        data_structures::SimpleCycle<int> cycle(v.begin(), v.end());
        int startLen = simple_algo::getLength(mtx, cycle);

        //creating local search
        auto lsc = getDefaultTwoLocalComponents(mtx);
        typedef GainCutSmallImproves<puretype(lsc.gain()), int> CIC;
        double epsilon = 0.001;
        CIC  cut(std::move(lsc.gain()), startLen, epsilon);
        auto cutLsc = swapGain(lsc, std::move(cut));
        auto lsCut = TwoLocalSearchStep<decltype(cycle), decltype(cutLsc)>(std::move(cycle), std::move(cutLsc));

#ifdef LOGGER_ON
        //printing 
        auto const & cman = lsCut.getSolution();
        LOG("Graph:\t" << fname);
        LOG("Length before\t" << simple_algo::getLength(mtx, cman));
        int i = 0;
#endif

        //search
        for(int j = 0; j < 20; ++j) {
            epsilon /= 2;
            LOG("epsilon = " << epsilon);
            lsCut.getSearchComponents().gain().setEpsilon(epsilon);
            while(lsCut.search()) {
                LOG("Length after\t" << i++ << ": " << simple_algo::getLength(mtx, cman));
            }
        }

        LOG("Normal search at the end");
        auto ls = TwoLocalSearchStep<decltype(cycle), decltype(lsc)>(std::move(lsCut.getSolution()), std::move(lsc));
        while(ls.search()) {
            LOG("Length after\t" << i++ << ": " << simple_algo::getLength(mtx, cman));
        }
    }
}

