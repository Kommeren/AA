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

//! [Local Search Components Example]
namespace ls =  paal::local_search;
using namespace  paal;

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
    int operator()(int s, int u) {
        return f(s + u) - f(s);
    }
};

struct Commit {
    void operator()(int & s, int u) {
        s = s + u;
    }
};

typedef  ls::SearchComponents<GetMoves, Gain, Commit> SearchComp;

//! [Local Search Components Example]

int main() {
//! [Local Search Example]
   //creating solution
   int solution(0);

   //search
   local_search_simple(solution, SearchComp());

   //print
   std::cout << "Local search solution: " <<  solution << std::endl;
//! [Local Search Example]
  return 0; 
}

