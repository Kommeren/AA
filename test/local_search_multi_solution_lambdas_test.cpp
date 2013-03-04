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

typedef boost::integer_range<int> Solution;
typedef  int SolutionElement;
typedef  SolutionElement Update;


/*struct GetNeigh {
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


void fillRand(Solution &s) {
    const int MAX_VAL = 10000;
    for(float & el : s) {
        el = float(std::rand() % MAX_VAL) / float(MAX_VAL); 
    }
}*/

BOOST_AUTO_TEST_CASE(local_search__multi_lamdas_choose_first_better_test) {
    //creating local search
/*    std::vector<float> x(DIM, 0);
    fillRand(x);

    float & x1(x[0]), & x2(x[1]), & x3(x[2]);
    float G;
    const std::vector<float> neighb{.01, -.01, .001, -.001};

    auto f = [&](Solution & _){ 
        return x1 *x2 +  x2 * x3 + x3 * x1 - 3 * x1 * x2 * x3 + G * (2- (x1 + x2 + x3));
    }

    auto gain [&](Solution & s, SolutionElement i, Update u) {
        auto old = x[i];
        auto val = f(s);
        x[i] = u;
        auto valUpdate = f(s);
        x[i] = old;
        return valUpdate - val;
    }

    auto us = [&](Solution & _, SolutionElement se, Update u) {
        x[se] = u;
    }
        
    typedef  local_search::MultiSearchComponents<GetNeigh, decltype(gain), decltype(su)> SearchComp;
    local_search::LocalSearchStepMultiSolution<Solution, SearchComp> ls(boost::irange(0,3));

    //printing
    auto const & s = ls.getSolution();
    LOG("f(");
    LOG_COPY_DEL(s.begin(), s.end(), ","); 
    LOG(") \t" << f(s));
    ON_LOG(int i = 0);

    //search
    while(ls.search()) {
        //printing
        LOG("f(");
        LOG_COPY_DEL(s.begin(), s.end(), ","); 
        LOG(") \t" << f(s) << " after " << i++ );
    }*/
    /*   BOOST_CHECK_EQUAL(s, 6);*/
}

