/**
 * @file local_search_example.cpp
 * @brief local search example
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-04
 */

#include <vector>
#include <iostream>

#include "paal/local_search/local_search.hpp"

//! [Local Search components Example]
namespace ls =  paal::local_search;
using namespace  paal;

int f(int x) {
    return -x*x + 12 * x -27;
}

struct get_moves {
    typedef typename std::vector<int>::const_iterator Iter;
    const std::vector<int> neighb;
    public:

    get_moves() : neighb{10, -10, 1, -1} {}

    const std::vector<int> & operator()(int x)  const {
        return neighb;
    }
};

struct Gain {
    int operator()(int s, int u) {
        return f(s + u) - f(s);
    }
};

struct Commit {
    bool operator()(int & s, int u) {
        s = s + u;
        return true;
    }
};

typedef  ls::search_components<get_moves, Gain, Commit> search_comps;

//! [Local Search components Example]

int main() {
    //! [Local Search Example]
    //creating solution
    int solution(0);

    //search
    local_search_simple(solution, search_comps());

    //print
    std::cout << "Local search solution: " <<  solution << std::endl;
    //! [Local Search Example]
    return 0;
}

