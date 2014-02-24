/**
 * @file simple_single_local_search_components.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2014-01-07
 */
#ifndef SIMPLE_SINGLE_LOCAL_SEARCH_COMPONENTS_HPP
#define SIMPLE_SINGLE_LOCAL_SEARCH_COMPONENTS_HPP

#include "paal/local_search/local_search.hpp"

namespace {
int f(int x) {
    return -x*x + 12 * x -27;
}

struct GetMoves {
    typedef typename std::vector<int>::const_iterator Iter;
    const std::vector<int> neighb;
public:

    GetMoves() : neighb{10, -10, 1, -1} {}

    std::pair<Iter, Iter> operator()(int x) {
        return std::make_pair(neighb.begin(), neighb.end());
    }
};

struct Gain {
    int operator()(int s, int u) const {
        return f(s + u) - f(s);
    }
};

struct Commit {
    bool operator()(int & s, int u) {
        s = s + u;
        return true;
    }
};

typedef  paal::local_search::SearchComponents<GetMoves, Gain, Commit> SearchComp;
} //anonymous namespace

#endif /* SIMPLE_SINGLE_LOCAL_SEARCH_COMPONENTS_HPP */
