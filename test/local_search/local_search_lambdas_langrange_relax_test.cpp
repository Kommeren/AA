//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file local_search_multi_solution_test.cpp
 * @brief this is implementation using lagrangian relaxation and c++11 lambdas
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-04
 */

#include "test_utils/logger.hpp"

#include "paal/local_search/local_search.hpp"
#include "paal/local_search/search_components.hpp"
#include "paal/data_structures/combine_iterator.hpp"
#include "paal/utils/irange.hpp"

#include <boost/test/unit_test.hpp>

#include <vector>
#include <string>
#include <cstdlib>
#include <iomanip>

using namespace paal;

struct value_diff {
    value_diff(double &value, double diff) : m_value(value), m_diff(diff) {}

    double &m_value;
    double m_diff;
};

struct make_value_diff {
    value_diff operator()(double &value, double diff) const {
        return value_diff{ value, diff };
    }
};

BOOST_AUTO_TEST_CASE(local_search_multi_lamdas_first_improving_test) {
    typedef double SolutionElement;
    typedef std::vector<SolutionElement> Solution;
    typedef SolutionElement Move;
    const int DIM = 3;
    const double LOWER_BOUND = 0.;
    const double UPPER_BOUND = 1.;

    // creating local search
    const std::vector<double> neighb{ .1, -.1, .01, -.01, .001, -.001 };
    std::vector<double> neighbCut(neighb.size());
    double G{ 1 };

    // components for vector
    auto f = [&](Solution & x) {
        double &x1(x[0]), &x2(x[1]), &x3(x[2]);
        return x1 * x2 + x2 * x3 + x3 * x1 - 3 * x1 * x2 * x3 +
               G * (2 - (x1 + x2 + x3));
    };

    auto normalize = [ = ](SolutionElement el) {
        return std::min(std::max(el, LOWER_BOUND), UPPER_BOUND);
    };

    auto getMoves = [&](Solution & s) {
        auto b = data_structures::make_combine_iterator(make_value_diff{}, s,
                                                        neighb);
        return boost::make_iterator_range(b, decltype(b) {});
    };

    auto gain = [&](Solution & s, value_diff vd) {
        auto old = vd.m_value;
        auto val = f(s);
        vd.m_value = normalize(vd.m_value + vd.m_diff);
        auto valMove = f(s);
        vd.m_value = old;
        return valMove - val - 1e-6;
    };

    auto commit = [&](Solution &, value_diff vd) {
        vd.m_value = normalize(vd.m_value + vd.m_diff);
        return true;
    };

    auto ls = [ = ](Solution & x) {
        x = { .3, .3, .3 };
        first_improving(
            x, local_search::make_search_components(getMoves, gain, commit));
    };

    // components for G.
    std::vector<double> neighbCutG(neighb.size());
    std::vector<double> x(DIM, 0);
    first_improving(
        x, local_search::make_search_components(getMoves, gain, commit));
    double best = f(x);

    auto getMovesG = [&](const double g)->const std::vector<double> & {
        for (int j : paal::irange(neighb.size())) {
            neighbCutG[j] = neighb[j] + g;
        }
        return neighbCutG;
    };

    auto gainG = [&](double, double g) {
        std::vector<double> x(DIM, 0);
        auto old = G;
        G = g;
        ls(x);
        auto newRes = f(x);
        G = old;
        return best - newRes - 1e-6;
    };

    auto commitG = [&](double & s, double u) {
        assert(G == s);
        s = u;
        best = f(x);
        return true;
    };

    first_improving(
        G, local_search::make_search_components(getMovesG, gainG, commitG));

    ls(x);

    // printing
    LOG(std::setprecision(10));
    LOGLN("G = " << G);
    G = 0;
    LOG("f(");
    LOG_COPY_RANGE_DEL(x, ",");
    // TODO it would be interesting how G depends on starting point ( (0.3, 0.3,
    // 0.3) now)
    LOGLN(") = \t" << f(x));
    LOGLN("approximation " << 2. / f(x));
}
