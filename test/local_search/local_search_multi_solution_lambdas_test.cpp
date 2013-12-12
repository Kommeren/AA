/**
 * @file local_search_multi_solution_test.cpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-04
 */

#include <vector>
#include <string>
#include <cstdlib>

#include <boost/test/unit_test.hpp>
#include <boost/range/irange.hpp>

#include "paal/local_search/local_search.hpp"
#include "paal/local_search/search_components.hpp"
#include "utils/logger.hpp"

using std::string;
using std::vector;
using namespace  paal;
const int  DIM = 3;
const double LOWER_BOUND = 0.;
const double UPPER_BOUND = 1.;

typedef  double SolutionElement;
typedef std::vector<SolutionElement> Solution;
typedef  SolutionElement Move;


void fillRand(Solution &s) {
    const int MAX_VAL = 10000;
    for(auto & el : s) {
        el = double(std::rand() % MAX_VAL) / double(MAX_VAL); 
    }
}

BOOST_AUTO_TEST_CASE(local_search__multi_lamdas_choose_first_better_test) {
    //TODO does not compile on clang
    //creating local search
    /*std::vector<double> x(DIM, 0);
    fillRand(x);

    double & x1(x[0]), & x2(x[1]), & x3(x[2]);
    double G;
    const std::vector<double> neighb{.01, -.01, .001, -.001};
    std::vector<double> neighbCut;

    auto f = [&](Solution & _){ 
        return x1 *x2 +  x2 * x3 + x3 * x1 - 3 * x1 * x2 * x3 + G * (2- (x1 + x2 + x3));
    };
    
    auto getMoves =[&] (const Solution & s, SolutionElement i) {
        for(int j : boost::irange(size_t(0), neighb.size())) {
            neighbCut[j] = std::max(neighb[j] + i, LOWER_BOUND);
            neighbCut[j] = std::min(neighbCut[j], UPPER_BOUND);
        }
        return std::make_pair(neighbCut.begin(), neighbCut.end());
    };

    auto gain = [&](Solution & s, SolutionElement i, Move u) {
        auto old = x[i];
        auto val = f(s);
        x[i] = u;
        auto valMove = f(s);
        x[i] = old;
        return valMove - val;
    };

    auto commit = [&](Solution & _, SolutionElement se, Move u) {
        x[se] = u;
    };
        
    local_search_multi_solution_simple(x, local_search::make_SearchComponents(getMoves, gain, commit));

    //printing
    LOGLN("f(");
    LOG_COPY_DEL(x.begin(), x.end(), ","); 
    LOGLN(") = \t" << f(x));
    BOOST_CHECK_EQUAL(f(x), 6.);*/
}

