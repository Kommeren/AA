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
#include <iomanip>

#include <boost/test/unit_test.hpp>
#include <boost/range/irange.hpp>

#include "paal/local_search/local_search.hpp"
#include "paal/local_search/search_components.hpp"
#include "paal/data_structures/combine_iterator.hpp"
#include "utils/logger.hpp"

using namespace  paal;


    struct ValueDiff {
        ValueDiff(double & value, double diff) :
            m_value(value), m_diff(diff) {}
        double & m_value;
        double m_diff;
    };

    struct MakeValueDiff {
        ValueDiff operator()(double & value, double diff) const {
            return ValueDiff{value, diff};
        }
    };

BOOST_AUTO_TEST_CASE(local_search_multi_lamdas_choose_first_better_test) {
    typedef  double SolutionElement;
    typedef std::vector<SolutionElement> Solution;
    typedef  SolutionElement Move;
    const int  DIM = 3;
    const double LOWER_BOUND = 0.;
    const double UPPER_BOUND = 1.;


    //creating local search
    const std::vector<double> neighb{0.1, -0.1, .01, -.01, .001, -.001};
    std::vector<double> neighbCut(neighb.size());
    double G{1};

    //components for vector
    auto f = [&](Solution & x){
        double & x1(x[0]), & x2(x[1]), & x3(x[2]);
        return x1 *x2 +  x2 * x3 + x3 * x1 - 3 * x1 * x2 * x3 + G * (2- (x1 + x2 + x3));
    };

    auto normalize = [=](SolutionElement el) {
        el = std::max(el, LOWER_BOUND);
        return std::min(el, UPPER_BOUND);
    };

    auto getMoves =[&] (Solution & s) {
        auto b = data_structures::make_CombineIterator(MakeValueDiff{}, s, neighb);
        return std::make_pair(b, decltype(b){});
    };

    auto gain = [&](Solution & s, ValueDiff vd) {
        auto old = vd.m_value;
        auto val = f(s);
        vd.m_value = normalize(vd.m_value + vd.m_diff);
        auto valMove = f(s);
        vd.m_value = old;
        return valMove - val - 0.000001;
    };

    auto commit = [&](Solution &, ValueDiff vd) {
        vd.m_value = normalize(vd.m_value + vd.m_diff);
        return true;
    };

    auto ls = [=](Solution & x) {
        x = {0.3,0.3,0.3};
        local_search_simple(x,
            local_search::make_SearchComponents(getMoves, gain, commit));
    };

    //components for G.
    std::vector<double> neighbCutG(neighb.size());
    std::vector<double> x(DIM, 0);
    local_search_simple(x,
                local_search::make_SearchComponents(getMoves, gain, commit));
    double best = f(x);


    auto getMovesG =[&] (const double g) {
        for(int j : boost::irange(std::size_t(0), neighb.size())) {
            neighbCutG[j] = neighb[j] + g;
        }
        return std::make_pair(neighbCutG.begin(), neighbCutG.end());
    };

    auto gainG = [&](double, double g) {
        std::vector<double> x(DIM, 0);
        auto old = G;
        G = g;
        ls(x);
        auto newRes = f(x);
        G = old;
        return best - newRes - 0.000001;
    };

    auto commitG = [&](double & s, double u) {
        assert(G == s);
        s = u;
        best = f(x);
        return true;
    };

    local_search_simple(G, local_search::make_SearchComponents(getMovesG, gainG, commitG));

    ls(x);

    //printing
    LOG(std::setprecision(10));
    LOGLN("G = " << G);
    G = 0;
    LOG("f(");
    LOG_COPY_DEL(x.begin(), x.end(), ",");
    //TODO it would be interesting how G depends on starting point ( (0.3, 0.3, 0.3) now)
    LOGLN( ") = \t" << f(x));
    LOGLN("approximation " << 2./f(x));
}

