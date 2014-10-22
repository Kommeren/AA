//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file local_search_multi_solution_test.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-04
 */

#include "utils/logger.hpp"

#include "paal/local_search/local_search.hpp"
#include "paal/data_structures/combine_iterator.hpp"

#include <boost/test/unit_test.hpp>

#include <vector>
#include <string>
#include <cstdlib>

using std::string;
using std::vector;
using namespace paal;
const int DIM = 3;
const float LOWER_BOUND = 0;
const float UPPER_BOUND = 1;

typedef std::vector<float> Solution;
typedef float SolutionElement;
typedef SolutionElement Move;

struct value_diff {
    value_diff(SolutionElement &value, float diff)
        : m_value(&value), m_diff(diff) {}
    value_diff() = default;
    SolutionElement *m_value;
    float m_diff;
};

namespace {
float f(const Solution &x) {
    float x1(x.at(0)), x2(x.at(1)), x3(x.at(2));
    return x1 * x2 + x2 * x3 + x3 * x1 - 3 * x1 * x2 * x3;
}

struct make_value_diff {
    value_diff operator()(SolutionElement &value, float diff) const {
        return value_diff{ value, diff };
    }
};

SolutionElement normalize(SolutionElement el) {
    return std::min(std::max(el, LOWER_BOUND), UPPER_BOUND);
}

struct get_moves {
    typedef std::vector<Move> Neigh;
    using Iter = paal::data_structures::combine_iterator<make_value_diff,
                                                         Solution, const Neigh>;
    using IterPair = boost::iterator_range<Iter>;

    const Neigh neighb;

  public:

    get_moves() : neighb{ .01, -.01, .001, -.001 } {}

    IterPair operator()(Solution &s) {
        auto b = data_structures::make_combine_iterator(make_value_diff{}, s,
                                                        neighb);
        return boost::make_iterator_range(b, decltype(b) {});
    }
};

struct Gain {
    float operator()(Solution &s, value_diff vd) {
        auto &el = *vd.m_value;
        auto old = el;
        auto val = f(s);
        el = normalize(*vd.m_value + vd.m_diff);
        auto valMove = f(s);
        el = old;
        return valMove - val;
    }
};

struct Commit {
    bool operator()(Solution &s, value_diff vd) {
        *vd.m_value = normalize(*vd.m_value + vd.m_diff);
        return true;
    }
};

typedef local_search::search_components<get_moves, Gain, Commit> search_comps;

void fill_rand(Solution &s) {
    const int MAX_VAL = 10000;
    for (float &el : s) {
        el = float(std::rand() % MAX_VAL) / float(MAX_VAL);
    }
}

ON_LOG(int i;)

bool logger(const Solution &s) {
    // printing
    LOGLN("f(");
    LOG_COPY_RANGE_DEL(s, ",");
    LOGLN(") \t" << f(s) << " after " << i++);
    return true;
}

} // anonymous namespace

BOOST_AUTO_TEST_CASE(local_search_first_improving_test) {
    // creating local search
    Solution sol(DIM, 0);
    fill_rand(sol);

    // printing
    LOGLN("f(");
    LOG_COPY_RANGE_DEL(sol, ",");
    LOGLN(") \t" << f(sol));
    ON_LOG(i = 0);

    // search
    local_search::local_search(sol, local_search::first_improving_strategy{},
                               logger, utils::always_false(), search_comps());
    BOOST_CHECK_EQUAL(f(sol), 1.);
}

BOOST_AUTO_TEST_CASE(local_search_best_improving_test) {
    // creating local search
    Solution sol(DIM, 0);
    fill_rand(sol);

    // printing
    LOGLN("f(");
    LOG_COPY_RANGE_DEL(sol, ",");
    LOGLN(") \t" << f(sol));
    ON_LOG(i = 0);

    // search
    local_search::local_search(sol, local_search::best_improving_strategy{},
                               logger, utils::always_false(), search_comps());
    BOOST_CHECK_EQUAL(f(sol), 1.);
}
