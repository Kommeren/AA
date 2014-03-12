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
#include "paal/data_structures/combine_iterator.hpp"
#include "utils/logger.hpp"

using std::string;
using std::vector;
using namespace  paal;
const int  DIM = 3;
const float LOWER_BOUND = 0;
const float UPPER_BOUND = 1;

typedef  std::vector<float> Solution;
typedef  float SolutionElement;
typedef  SolutionElement Move;

struct ValueDiff {
    ValueDiff(SolutionElement & value, float diff) :
        m_value(&value), m_diff(diff) {}
    ValueDiff() = default;
    SolutionElement * m_value;
    float m_diff;
};

namespace {
float f(const Solution & x) {
    float x1(x.at(0)), x2(x.at(1)), x3(x.at(2));
    return x1 *x2 +  x2 * x3 + x3 * x1 - 3 * x1 * x2 * x3;
}


struct MakeValueDiff {
    ValueDiff operator()(SolutionElement & value, float diff) const {
        return ValueDiff{value, diff};
    }
};

SolutionElement normalize(SolutionElement el) {
    el = std::max(el, LOWER_BOUND);
    return std::min(el, UPPER_BOUND);
}


struct GetMoves {
    typedef std::vector<Move> Neigh;
    using Iter = paal::data_structures::CombineIterator<MakeValueDiff, Solution, const Neigh>;
    using IterPair = std::pair<Iter, Iter>;

    const Neigh neighb;
public:

    GetMoves() : neighb{.01, -.01, .001, -.001} {}

    IterPair operator()(Solution & s) {
        auto b = data_structures::make_CombineIterator(MakeValueDiff{}, s, neighb);
        return std::make_pair(b, decltype(b){});
    }
};

struct Gain {
    float operator()(Solution & s, ValueDiff vd) {
        auto & el = *vd.m_value;
        auto old = el;
        auto val = f(s);
        el = normalize(*vd.m_value + vd.m_diff);
        auto valMove = f(s);
        el = old;
        return valMove - val;
    }
};

struct Commit {
    bool operator()(Solution & s, ValueDiff vd) {
        *vd.m_value = normalize(*vd.m_value + vd.m_diff);
        return true;
    }
};

typedef  local_search::SearchComponents<GetMoves, Gain, Commit> SearchComp;

void fillRand(Solution &s) {
    const int MAX_VAL = 10000;
    for(float & el : s) {
        el = float(std::rand() % MAX_VAL) / float(MAX_VAL);
    }
}

ON_LOG(
int i;
)

auto logger = [&](const Solution & s) {
    //printing
    LOGLN("f(");
    LOG_COPY_RANGE_DEL(s, ",");
    LOGLN(") \t" << f(s) << " after " << i++ );
};
} //anonymous namespace


BOOST_AUTO_TEST_CASE(local_search_choose_first_better_test) {
    //creating local search
    Solution sol(DIM, 0);
    fillRand(sol);

    //printing
    LOGLN("f(");
    LOG_COPY_RANGE_DEL(sol, ",");
    LOGLN(") \t" << f(sol));
    ON_LOG(i = 0);

    //search
    local_search::local_search(sol, logger, utils::ReturnFalseFunctor(), SearchComp());
    BOOST_CHECK_EQUAL(f(sol), 1.);
}


BOOST_AUTO_TEST_CASE(local_search_steepest_slope_test) {
    //creating local search
    Solution sol(DIM, 0);
    fillRand(sol);

    //printing
    LOGLN("f(");
    LOG_COPY_RANGE_DEL(sol, ",");
    LOGLN(") \t" << f(sol));
    ON_LOG(i = 0);

    //search
    local_search::local_search<local_search::search_strategies::SteepestSlope>
        (sol, logger, utils::ReturnFalseFunctor(), SearchComp());
    BOOST_CHECK_EQUAL(f(sol), 1.);
}
