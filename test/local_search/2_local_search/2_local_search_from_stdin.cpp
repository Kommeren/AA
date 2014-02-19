/**
 * @file 2_local_search_from_stdin.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-09-20
 */
#define BOOST_TEST_MODULE two_local_search_stdin

#include <boost/test/unit_test.hpp>

#include <vector>
#include <string>

#include "paal/local_search/2_local_search/2_local_search.hpp"
#include "paal/data_structures/cycle/simple_cycle.hpp"
#include "paal/data_structures/cycle/splay_cycle.hpp"
#include "paal/local_search/custom_components.hpp"
#include "paal/data_structures/components/components_replace.hpp"

#include "utils/read_tsplib.h"
#include "2_local_search_logger.hpp"

using std::vector;
using namespace paal::local_search::two_local_search;
using namespace paal;

std::string path = "test/data/TSPLIB/symmetrical/";

template <typename Cycle>
void test() {
    read_tsplib::TSPLIB_Directory::Graph graph(std::cin);
    read_tsplib::TSPLIB_Matrix mtx;

    graph.load(mtx);
    auto size = mtx.size();
    std::vector<int> v(size);
    std::iota(v.begin(), v.end(), 0);

    //create random solution
    std::random_shuffle(v.begin(), v.end());
    LOG_COPY_RANGE_DEL(v, ",");
    Cycle cycle(v.begin(), v.end());
    LOG_COPY_DEL(cycle.vbegin(), cycle.vend(), ",");

    //printing
    LOG_COPY_DEL(cycle.vbegin(), cycle.vend(), ",");
    LOGLN("Length before\t" << simple_algo::getLength(mtx, cycle));

    //setting logger
    auto logger = utils::make_twoLSLogger(mtx);

    //search
    two_local_search(cycle, logger, utils::ReturnFalseFunctor(), getDefaultTwoLocalComponents(mtx));
}

BOOST_AUTO_TEST_CASE(TSPLIB_simple) {
    test<paal::data_structures::SimpleCycle<int>>();
}

BOOST_AUTO_TEST_CASE(TSPLIB) {
    test<paal::data_structures::SplayCycle<int>>();
}

using paal::local_search::SearchComponents;


//TODO probably optimization does not optimize...
BOOST_AUTO_TEST_CASE(TSPLIB_cut) {
    read_tsplib::TSPLIB_Directory::Graph graph(std::cin);
    read_tsplib::TSPLIB_Matrix mtx;
    graph.load(mtx);
    auto size = mtx.size();
    std::vector<int> v(size);
    std::iota(v.begin(), v.end(), 0);

    //create random solution
    typedef data_structures::SimpleCycle<int> Cycle;
    std::random_shuffle(v.begin(), v.end());
    Cycle cycle(v.begin(), v.end());
    int startLen = simple_algo::getLength(mtx, cycle);

    //creating local search
    auto lsc = getDefaultTwoLocalComponents(mtx);
    typedef local_search::SearchComponentsTraits<puretype(lsc)>::GainT GainT;
    typedef local_search::GainCutSmallImproves<GainT, int> CIC;
    double epsilon = 0.001;
    CIC  cut(lsc.get<paal::local_search::Gain>(), startLen, epsilon);
    auto cutLsc = data_structures::replace<local_search::Gain>(std::move(cut), lsc);

    //setting logger
    auto logger = utils::make_twoLSLogger(mtx);

    //printing
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

