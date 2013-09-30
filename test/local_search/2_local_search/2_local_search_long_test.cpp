#define BOOST_TEST_MODULE two_local_search_long

#include <boost/test/unit_test.hpp>

#include <vector>
#include <string>
#include <iomanip>

#include "paal/local_search/2_local_search/2_local_search.hpp"
#include "paal/data_structures/cycle/simple_cycle.hpp"
#include "paal/data_structures/components/components_replace.hpp"
#include "paal/local_search/custom_components.hpp"

#include "utils/read_tsplib.h"
#include "2_local_search_logger.hpp"

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

        //printing 
        LOGLN("Graph:\t" << fname);
        LOGLN("Length before\t" << simple_algo::getLength(mtx, cycle));

        
        //setting logger
        auto logger = utils::make_twoLSLogger(mtx, 100);

        //search
        two_local_search(cycle, logger, utils::ReturnFalseFunctor(), lsc);
        LOGLN(std::setprecision(20) << "APPROXIMATION RATIO: " << float(simple_algo::getLength(mtx, cycle)) / opt);
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
        typedef local_search::SearchComponentsTraits<puretype(lsc)>::GainT GainT;
        typedef paal::local_search::GainCutSmallImproves<GainT, int> CIC;
        double epsilon = 0.001;
        CIC  cut(lsc.get<local_search::Gain>(), startLen, epsilon);
        auto cutLsc = data_structures::replace<local_search::Gain>(std::move(cut), lsc);
        
        //setting logger
        auto logger = utils::make_twoLSLogger(mtx, 100);

        //printing 
        LOGLN("Graph:\t" << fname);
        LOGLN("Length before\t" << simple_algo::getLength(mtx, cycle));

        //search
        for(int j = 0; j < 20; ++j) {
            epsilon /= 2;
            LOGLN("epsilon = " << epsilon);
            cutLsc.get<local_search::Gain>().setEpsilon(epsilon);
            two_local_search(cycle, logger, utils::ReturnFalseFunctor(), cutLsc);
        }

        LOGLN("Normal search at the end");
        two_local_search(cycle, logger, utils::ReturnFalseFunctor(), lsc);
    }
}

