/**
 * @file local_search_example.cpp
 * @brief local search example 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-04
 */

#include <vector>
#include <iostream>

#include "paal/local_search/local_search_step.hpp"

//! [Local Search Components Example]
namespace ls =  paal::local_search;
using namespace  paal;

int f(int x) {
    return -x*x + 12 * x -27;
}

struct GetNeigh {
    typedef typename std::vector<int>::const_iterator Iter;
    const std::vector<int> neighb;
public:

    GetNeigh() : neighb{10, -10, 1, -1} {}

    std::pair<Iter, Iter> operator()(int x) {
        return std::make_pair(neighb.begin(), neighb.end());
    }
};

struct Gain {
    int operator()(int s, int u) {
        return f(s + u) - f(s);
    }
};

struct UpdateSolution {
    void operator()(int & s, int u) {
        s = s + u;
    }
};

typedef  ls::SearchComponents<GetNeigh, Gain, UpdateSolution> SearchComp;

//! [Local Search Components Example]

int main() {
//! [Local Search Example]
   //creating local search
    ls::LocalSearchStep<int, ls::search_strategies::ChooseFirstBetter, SearchComp> ls;

   //search
   search(ls);
   std::cout << "Local search solution: " <<  ls.getSolution() << std::endl;
//! [Local Search Example]
  return 0; 
}

