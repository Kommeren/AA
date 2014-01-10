/**
 * @file local_search_multi_solution_test.cpp
 * @brief this is implementation using lagrangian relaxation and c++11 lambdas 
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

using namespace  paal;

BOOST_AUTO_TEST_CASE(local_search_multi_lamdas_choose_first_better_test) {
    typedef  double SolutionElement;
    typedef std::vector<SolutionElement> Solution;
    typedef  SolutionElement Move;
    const int  DIM = 3;
    const double LOWER_BOUND = 0.;
    const double UPPER_BOUND = 1.;

    //creating local search

    const std::vector<double> neighb{.01, -.01, .001, -.001};
    std::vector<double> neighbCut(4);
    double G{1};

    //components for vector
    auto f = [&](Solution & x){ 
        double & x1(x[0]), & x2(x[1]), & x3(x[2]);
        return x1 *x2 +  x2 * x3 + x3 * x1 - 3 * x1 * x2 * x3 + G * (2- (x1 + x2 + x3));
    };
   
    auto getMoves =[&] (const Solution & s, SolutionElement i) {
        for(int j : boost::irange(size_t(0), neighb.size())) {
            neighbCut[j] = std::max(neighb[j] + i, LOWER_BOUND);
            neighbCut[j] = std::min(neighbCut[j], UPPER_BOUND);
        }
        return std::make_pair(neighbCut.begin(), neighbCut.end());
    };

    auto gain = [&](Solution & s, SolutionElement & i, Move u) {
        auto old = i;
        auto val = f(s);
        i = u;
        auto valMove = f(s);
        i = old;
        return valMove - val;
    };

    auto commit = [&](Solution &, SolutionElement & se, Move u) {
        se = u;
    };
        
    auto ls = [=](Solution & x) {
        local_search_multi_solution_simple(x, 
            local_search::make_SearchComponents(getMoves, gain, commit));
    };

    //components for G.
    std::vector<double> neighbCutG(4);
    std::vector<double> x(DIM, 0);
    local_search_multi_solution_simple(x, 
                local_search::make_SearchComponents(getMoves, gain, commit)); 
    double best = f(x); 


    auto getMovesG =[&] (const double g) {
        for(int j : boost::irange(size_t(0), neighb.size())) {
            neighbCutG[j] = neighb[j] + g;
        }
        return std::make_pair(neighbCutG.begin(), neighbCutG.end());
    };
    
    auto gainG = [&](double, double g) {
        std::vector<double> x(DIM, 0);
        auto old = G;
        G = g;
        ls(x); 
        G = old;
        return best - f(x);
    };
    
    auto commitG = [&](double & s, double u) {
        s = u;
    };
            
    local_search_simple(G, local_search::make_SearchComponents(getMovesG, gainG, commitG));
    ls(x);

    //printing
    LOGLN("f(");
    LOG_COPY_DEL(x.begin(), x.end(), ","); 
    LOGLN(") = \t" << f(x));
    //TODO, unfinished invesitigate
//    BOOST_CHECK_EQUAL(f(x), 6.);
}

