/**
 * @file local_search_multi_solution_test.cpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-04
 */

#define BOOST_TEST_MODULE local_search_multi_solution
#include <vector>
#include <string>
#include <cstdlib>

#include <boost/test/unit_test.hpp>
#include <boost/range/irange.hpp>

#include "paal/local_search/local_search.hpp"
#include "utils/logger.hpp"

using std::string;
using std::vector;
using namespace  paal;
const int  DIM = 3;
const float LOWER_BOUND = 0;
const float UPPER_BOUND = 1;

typedef  std::vector<float> Solution;
typedef  float SolutionElement;
typedef  SolutionElement Update;

float f(const Solution & x) {
    float x1(x.at(0)), x2(x.at(1)), x3(x.at(2));
    return x1 *x2 +  x2 * x3 + x3 * x1 - 3 * x1 * x2 * x3;
}

struct GetNeigh {
    typedef std::vector<Update> Neigh;
    typedef typename Neigh::const_iterator Iter;
    const Neigh neighb;
    Neigh neighbCut;
public:

    GetNeigh() : neighb{.01, -.01, .001, -.001}, neighbCut(neighb.size()) {}

    std::pair<Iter, Iter> operator()(const Solution & s, SolutionElement i) {
        for(int j : boost::irange(size_t(0), neighb.size())) {
            neighbCut[j] = std::max(neighb[j] + i, LOWER_BOUND);
            neighbCut[j] = std::min(neighbCut[j], UPPER_BOUND);
        }
        return std::make_pair(neighbCut.begin(), neighbCut.end());
    }
};

struct Gain {
    float operator()(Solution & s, const SolutionElement & i, Update u) {
        auto & el = const_cast<SolutionElement &>(i);
        auto old = i;
        auto val = f(s);
        el = u;
        auto valUpdate = f(s);
        el = old;
        return valUpdate - val;
    }
};

struct UpdateSolution {
    void operator()(Solution & s, const SolutionElement & i, Update u) {
        auto & el = const_cast<SolutionElement &>(i);
        el = u;
    }
};

typedef  local_search::MultiSearchComponents<GetNeigh, Gain, UpdateSolution> SearchComp;
void fillRand(Solution &s) {
    const int MAX_VAL = 10000;
    for(float & el : s) {
        el = float(std::rand() % MAX_VAL) / float(MAX_VAL); 
    }
}

BOOST_AUTO_TEST_CASE(two_local_search_choose_first_better_test) {
    //creating local search
    Solution initSol(DIM, 0);
    fillRand(initSol);
    local_search::LocalSearchStepMultiSolution<Solution, SearchComp> ls(initSol);

    //printing
    auto const & s = ls.getSolution();
    LOG("f(");
    LOG_COPY_DEL(s.begin(), s.end(), ","); 
    LOG(") \t" << f(s));
#ifdef LOGGER_ON
    int i = 0;
#endif

    //search
    while(ls.search()) {
        //printing
        LOG("f(");
        LOG_COPY_DEL(s.begin(), s.end(), ","); 
        LOG(") \t" << f(s) << " after " << i++ );
    }
    /*   BOOST_CHECK_EQUAL(s, 6);*/
}


BOOST_AUTO_TEST_CASE(two_local_search_steepest_slope_test) {
    //creating local search
    Solution initSol(DIM, 0);
    fillRand(initSol);
    local_search::LocalSearchStepMultiSolution<Solution, SearchComp, local_search::search_strategies::SteepestSlope> ls(initSol);

    //printing 
    auto const & s = ls.getSolution();
    LOG("f(");
    LOG_COPY_DEL(s.begin(), s.end(), ","); 
    LOG(") \t" << f(s));
#ifdef LOGGER_ON
    int i = 0;
#endif

    //search
    while(ls.search()) {
        //printing
        LOG("f(");
        LOG_COPY_DEL(s.begin(), s.end(), ","); 
        LOG(") \t" << f(s) << " after " << i++ );
    }
    //   BOOST_CHECK_EQUAL(s, 6);
}
